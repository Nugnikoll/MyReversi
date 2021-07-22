import time
import os
import sys
import numpy as np
from scipy import optimize
import argparse

import data

sys.path.append("../python")
import reversi as rv

parser = argparse.ArgumentParser(
	description = "training parameters to estimate the upper bound and the lower bound"
)
parser.add_argument(
	"--path-train", type = str, default = "./train.dat",
	help = "path to load training data (default: ./train.dat)"
)
parser.add_argument(
	"--path-save", type = str, default = "./pattern_bound.dat",
	help = "path to save training result (default: ./pattern.dat)"
)
parser.add_argument(
	"--epoch", type = int, default = 400,
	help = "the number of epochs to run (default: 400)"
)
parser.add_argument(
	"--sample-alias", type = str, default = "sample",
	help = "alias of sample"
)
parser.add_argument(
	"--target-alias", type = str, default = "target",
	help = "alias of target"
)
args = parser.parse_args()

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf

rv.board.config()
rv.pattern.config()

name = args.path_train

if not os.path.exists(name):
	print("\"%s\" not found" % name)
	exit()

with open(name, "rb") as fobj:
	sample, target = data.load_npy_list(fobj, (
		"sample", "target"
	))

if sample is None:
	print("sample not found")
	exit()
print("sample shape", sample.shape)
if target is None:
	print("target not found")
	exit()
print("target shape", target.shape)
size = sample.shape[0]

ptn_shape = rv.pattern().view().shape
weight = np.zeros(ptn_shape, dtype = np.float64).ravel()

# parameters
k = 100
m = 0.00001
x0 = 0
def f(x):
	y = np.exp(k * (x - x0))
	return y / (1 + y) - m
x0 = -optimize.fsolve(f, [0])[0]
print(x0, f(0))

def f(x):
	flag = x < x0
	y = np.where(flag, np.exp(k * (x - x0)), np.exp(- k * (x - x0)))
	y1 = np.where(flag, y, 1)
	return x * (y1 / (1 + y) - m)

def df(x):
	flag = x < x0
	y = np.where(flag, np.exp(k * (x - x0)), np.exp(- k * (x - x0)))
	y1 = np.where(flag, y, 1)
	return (y1 / (1 + y) - m) + k * x * y / (1 + y) ** 2

def check_grad(f, df):
	x = (np.array(range(21)) - 10) / 5
	epsilon = 0.00001
	return ((f(x + epsilon) - f(x)) / epsilon) - df(x)

print("check_grad", check_grad(f, df))

# def f(x):
	# ones = np.ones(x.shape)
	# return (ones * a + (x > 0) * (b - a)) * x

# def df(x):
	# ones = np.ones(x.shape)
	# return (ones * a + (x > 0) * (b - a)).astype(np.float32)

# loss function
def fun(weight):
	global size
	value = rv.evaluate(rv.pattern(weight.astype(np.float32)), sample)
	delta = value - target
	loss = f(delta).mean()
	delta = df(delta)
	ptn_grad = rv.pattern()
	ptn_grad.initial()
	rv.adjust(ptn_grad, sample, delta)
	grad = ptn_grad.numpy()
	return (loss.astype(np.float64), (grad / size).astype(np.float64))

time_begin = time.time()
result = optimize.minimize(
	fun, weight, method = "L-BFGS-B", jac = True,
	options = {
		"disp": True,
		"maxcor": 25,
		"maxiter": args.epoch
	}
)
time_end = time.time()
print("time: ", time_end - time_begin)

ptn = rv.pattern(result.x.astype(np.float32))
ptn.balance()
ptn.save(args.path_save)

# check
value = rv.evaluate(ptn, sample)
value = (value > target).sum()
print(size, value, value / size)
