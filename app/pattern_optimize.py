import os
import sys
import numpy as np
from scipy import optimize
import argparse

import util
import data

sys.path.append("../python")
import reversi as rv

parser = argparse.ArgumentParser(
	description = "training parameters for reversi program"
)
parser.add_argument(
	"--path-train", type = str, default = "./train.dat",
	help = "path to load training data (default: ./train.dat)"
)
parser.add_argument(
	"--path-save", type = str, default = "./pattern.dat",
	help = "path to save training result (default: ./pattern.dat)"
)
parser.add_argument(
	"--epoch", type = int, default = 400,
	help = "the number of epochs to run (default: 400)"
)
parser.add_argument(
	"--pattern-type", type = str, default = "average",
	help = "the type of pattern should be \"average\", \"ubound\" or \"lbound\" (default: average)"
)
parser.add_argument(
	"--sample-alias", type = str, default = "sample",
	help = "alias of sample"
)
parser.add_argument(
	"--target-alias", type = str, default = "target",
	help = "alias of target"
)
parser.add_argument(
	"--pattern-alias", type = str, default = "pattern",
	help = "alias of pattern"
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
	sample, target = data.load_list(fobj, (
		args.sample_alias,
		args.target_alias,
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

if os.path.exists(args.path_save):
	with open(args.path_save, "rb") as fobj:
		head, (ptn,), data_list = data.load_list(
			fobj, (args.pattern_alias,), need_full = True
		)
		if not "append" in head:
			head["append"] = []
else:
	head = {"append":[]}
	ptn = None
	data_list = []

if not ptn is None:
	print("pattern exists")
	exit()

ptn_shape = rv.pattern().view().shape
# weight = np.zeros(ptn_shape, dtype = np.float64).ravel()
weight = (np.random.rand(rv.pattern.size) - 0.5) * 0.1

if args.pattern_type == "average":
	def f(x):
		return x ** 2

	def df(x):
		return 2 * x

elif args.pattern_type in ("lbound", "ubound"):
	# parameters
	k = 10
	m = 0.0001
	if args.pattern_type == "ubound":
		m = 1 - m
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
	return (f(x + epsilon) - f(x)) / epsilon - df(x)

print("check_grad", check_grad(f, df))

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

with util.timespan():
	result = optimize.minimize(
		fun, weight, method = "L-BFGS-B", jac = True,
		options = {
			"disp": True,
			"maxcor": 25,
			"maxiter": args.epoch,
		}
	)

ptn = rv.pattern(result.x.astype(np.float32))
ptn.balance()

head["append"].append({
	"name": args.pattern_alias,
	"type": "pattern",
	"info": {
		"type": args.pattern_type,
		"method": "L-BFGS-B",
		"maxiter": args.epoch,
		"sample": args.sample_alias,
		"target": args.target_alias,
	},
})
data_list.append(ptn)

value = rv.evaluate(ptn, sample)
ucount = (value > target).sum()
print(size, ucount, ucount / size)
print(target.min(), target.max(), target.mean())
print(value.min(), value.max(), value.mean())

with open(args.path_save, "wb") as fobj:
	data.dump_list(fobj, head, data_list, mode = "json")
