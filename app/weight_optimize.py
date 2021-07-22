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
	description = "training parameters for reversi program"
)
parser.add_argument(
	"--path-train", type = str, default = "./train.dat",
	help = "path to load training data (default: ./train.dat)"
)
parser.add_argument(
	"--path-save", type = str, default = "./weight.dat",
	help = "path to save training result (default: ./weight.dat)"
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

weight = np.zeros((65, 8), dtype = np.float64).ravel()
param = np.zeros((size, 8), dtype = np.float64)
count = np.empty(size, dtype = np.int32)

for i in range(size):
	brd = rv.board(sample[i])
	brd_black = brd.get_brd(True)
	brd_white = brd.get_brd(False)
	count[i] = brd.sum()
	param[i, 0] = (
		rv.board.count(brd_black & 0x8100000000000081) 
		- rv.board.count(brd_white & 0x8100000000000081)
	)
	param[i, 1] = (
		rv.board.count(brd_black & 0x7e8181818181817e) 
		- rv.board.count(brd_white & 0x7e8181818181817e)
	)
	param[i, 2] = (
		rv.board.count(brd_black & 0x0042000000004200) 
		- rv.board.count(brd_white & 0x0042000000004200)
	)
	param[i, 3] = (
		rv.board.count(brd_black & 0x003c7e7e7e7e3c00) 
		- rv.board.count(brd_white & 0x003c7e7e7e7e3c00)
	)
	pblack = brd.get_move_flip(True)
	pwhite = brd.get_move_flip(False)
	param[i, 4] = rv.board.count(pblack.first) - rv.board.count(pwhite.first)
	param[i, 5] = rv.board.count(brd_black & ~pwhite.second) - rv.board.count(brd_white & ~pblack.second)
	front = rv.board.get_front(brd_black | brd_white)
	param[i, 6] = rv.board.count(front & brd_black) - rv.board.count(front & brd_white)

def fun(weight):
	global size
	global param
	global count

	w = weight.reshape((65, 8))
	loss = 0
	grad = np.zeros(w.shape, dtype = np.float64)
	
	for i in range(size):
		p = param[i]
		wc = w[int(count[i])]
		value = p.dot(wc)
		diff = value - target[i]
		loss += diff ** 2
		grad[int(count[i])] += diff * p
	loss /= size
	grad = grad.ravel()
	grad = grad * 2 / size
	return (loss, grad)

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

w = result.x.astype(np.float32).reshape((65, 8))
with open(args.path_save, "wb") as fobj:
	np.save(fobj, w)
