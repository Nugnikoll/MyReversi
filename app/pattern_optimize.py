import time
import matplotlib.pyplot as plt
import os
import sys
import numpy as np
from scipy import optimize
import argparse

sys.path.append("../python")
import reversi as rv

parser = argparse.ArgumentParser(description = "training parameters for reversi programming")
parser.add_argument("--num-simulate", type = int, default = 10000, help = "number of random self-play games (default: 10000)")
parser.add_argument("--path-sample", type = str, default = "./sample.dat", help = "path to find sample (default: ./sample.dat) sample will be generated if it cannot be found here")
parser.add_argument("--path-target", type = str, default = "./target.dat", help = "path to find target of optimization (default: ./target) target will be generated if it cannot be found here")
parser.add_argument("--path-save", type = str, default = "./pattern.dat", help = "path to save training result (default: ./pattern.dat)")
parser.add_argument("--path-pattern", type = str, default = "", help = "generate target with old pattern and specify the path to find it")
parser.add_argument("--epoch", type = int, default = 400, help = "the number of epochs to run (default: 400)")
args = parser.parse_args()

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_mtdf

rv.board.config()
rv.pattern.config()

name = args.path_sample
if os.path.exists(name):
	print("sample already exists")
	with open(name, "rb") as fobj:
		sample = np.load(fobj)
else:
	print("generate sample")
	time_begin = time.time()
	sample = rv.sample_gen_select(args.num_simulate)
	time_end = time.time()
	print("time: ", time_end - time_begin)
	with open(name, "wb") as fobj:
		np.save(fobj, sample)

size = sample.shape[0]
print("sample size: ", size)

name = args.path_target
if os.path.exists(name):
	print("target already exists")
	with open(name, "rb") as fobj:
		target = np.load(fobj)
else:
	if os.path.exists(args.path_pattern):
		rv.pattern.config(args.path_pattern)
		mthd = mthd | rv.mthd_ptn
	print("generate target")
	time_begin = time.time()
	target = rv.evaluate(sample, mthd, 4)
	time_end = time.time()
	print("time: ", time_end - time_begin)
	with open(name, "wb") as fobj:
		np.save(fobj, target)

ptn_shape = rv.pattern().view().shape
weight = np.zeros(ptn_shape, dtype = np.float64).ravel()

def fun(weight):
	global size
	value = rv.evaluate(rv.pattern(weight.astype(np.float32)), sample)
	delta = value - target
	loss = (delta ** 2).mean()
	ptn_grad = rv.pattern()
	ptn_grad.initial()
	rv.adjust(ptn_grad, sample, delta)
	grad = ptn_grad.numpy()
	return (loss.astype(np.float64), (grad * 2 / size).astype(np.float64))

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
