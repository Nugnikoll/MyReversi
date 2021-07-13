import time
import os
import sys
import numpy as np
from scipy import optimize
import argparse

import data

sys.path.append("../python")
import reversi as rv

parser = argparse.ArgumentParser(description = "training parameters for reversi program")
parser.add_argument("--path-train", type = str, default = "./train.dat", help = "path to load training data (default: ./train.dat)")
parser.add_argument("--path-save", type = str, default = "./pattern.dat", help = "path to save training result (default: ./pattern.dat)")
parser.add_argument("--epoch", type = int, default = 400, help = "the number of epochs to run (default: 400)")
args = parser.parse_args()

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_mtdf

rv.board.config()
rv.pattern.config()

name = args.path_train

if not os.path.exists(name):
	print("\"%s\" not found" % name)
	exit()

with open(name, "rb") as fobj:
	sample, target, play_score = data.load_npy_list(
		fobj, ("sample", "target", "play_score")
	)

if sample is None:
	print("sample not found")
	exit()
print("sample shape", sample.shape)
if not play_score is None:
	target = play_score
if target is None:
	print("target not found")
	exit()
print("target shape", target.shape)
size = sample.shape[0]

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
