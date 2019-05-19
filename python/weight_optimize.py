import time;
import matplotlib.pyplot as plt;
import os;
import numpy as np;
from scipy import optimize;
import argparse;
import reversi as rv;

parser = argparse.ArgumentParser(description = "training parameters for reversi programming");
parser.add_argument("--num-simulate", type = int, default = 10000, help = "number of random self-play games (default: 10000)");
parser.add_argument("--path-sample", type = str, default = "./sample.dat", help = "path to find sample (default: ./sample.dat) sample will be generated if it cannot be found here");
parser.add_argument("--path-target", type = str, default = "./target.dat", help = "path to find target of optimization (default: ./target) target will be generated if it cannot be found here");
parser.add_argument("--path-save", type = str, default = "./weight.dat", help = "path to save training result (default: ./weight.dat)");
parser.add_argument("--path-pattern", type = str, default = "", help = "generate target with old pattern and specify the path to find it");
parser.add_argument("--epoch", type = int, default = 400, help = "the number of epochs to run (default: 400)");
args = parser.parse_args();

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_mtdf;

rv.board.config();
rv.pattern.config();

name = args.path_sample;
if os.path.exists(name):
	print("sample already exists");
	with open(name, "rb") as fobj:
		sample = np.load(fobj);
else:
	print("generate sample");
	time_begin = time.time();
	sample = rv.sample_gen(args.num_simulate);
	time_end = time.time();
	print("time: ", time_end - time_begin);
	with open(name, "wb") as fobj:
		np.save(fobj, sample);

size = sample.shape[0];
print("sample size: ", size);

name = args.path_target;
if os.path.exists(name):
	print("target already exists");
	with open(name, "rb") as fobj:
		target = np.load(fobj);
else:
	if os.path.exists(args.path_pattern):
		rv.pattern.config(args.path_pattern);
		mthd = mthd | rv.mthd_ptn;
	print("generate target");
	time_begin = time.time();
	target = rv.evaluate(sample, mthd, 4);
	time_end = time.time();
	print("time: ", time_end - time_begin);
	with open(name, "wb") as fobj:
		np.save(fobj, target);

weight = np.zeros((65, 8), dtype = np.float64).ravel();
param = np.zeros((size, 8), dtype = np.float64);
count = np.empty(size, dtype = np.int32);

for i in range(size):
	brd = rv.board(sample[i]);
	brd_black = brd.get_brd(True);
	brd_white = brd.get_brd(False);
	count[i] = brd.sum();
	param[i, 0] = (
		rv.board.count(brd_black & 0x8100000000000081) 
		- rv.board.count(brd_white & 0x8100000000000081)
	);
	param[i, 1] = (
		rv.board.count(brd_black & 0x7e8181818181817e) 
		- rv.board.count(brd_white & 0x7e8181818181817e)
	);
	param[i, 2] = (
		rv.board.count(brd_black & 0x0042000000004200) 
		- rv.board.count(brd_white & 0x0042000000004200)
	);
	param[i, 3] = (
		rv.board.count(brd_black & 0x003c7e7e7e7e3c00) 
		- rv.board.count(brd_white & 0x003c7e7e7e7e3c00)
	);
	pblack = brd.get_move_flip(True);
	pwhite = brd.get_move_flip(False);
	param[i, 4] = rv.board.count(pblack.first) - rv.board.count(pwhite.first);
	param[i, 5] = rv.board.count(brd_black & ~pwhite.second) - rv.board.count(brd_white & ~pblack.second);
	front = rv.board.get_front(brd_black | brd_white);
	param[i, 6] = rv.board.count(front & brd_black) - rv.board.count(front & brd_white);

def fun(weight):
	global size;
	global param;
	global count;

	w = weight.reshape((65, 8));
	loss = 0;
	grad = np.zeros(w.shape, dtype = np.float64);
	
	for i in range(size):
		p = param[i];
		wc = w[int(count[i])];
		value = p.dot(wc);
		diff = value - target[i];
		loss += diff ** 2;
		grad[int(count[i])] += diff * p;
	loss /= size;
	grad = grad.ravel();
	grad = grad * 2 / size;
	return (loss, grad);

time_begin = time.time();
result = optimize.minimize(
	fun, weight, method = "L-BFGS-B", jac = True,
	options = {
		"disp": True,
		"maxcor": 25,
		"maxiter": args.epoch
	}
)
time_end = time.time();
print("time: ", time_end - time_begin);

w = result.x.astype(np.float32).reshape((65, 8));
with open(args.path_save, "wb") as fobj:
	np.save(fobj, w);
