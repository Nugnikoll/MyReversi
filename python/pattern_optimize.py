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
parser.add_argument("--path-occur", type = str, default = "./occurrence.dat", help = "path to find count of occurrence (default: ./occurrence.dat) sample will be generated if it cannot be found here");
parser.add_argument("--path-target", type = str, default = "./target.dat", help = "path to find target of optimization (default: ./target) target will be generated if it cannot be found here");
parser.add_argument("--path-save", type = str, default = "./pattern.dat", help = "path to save training result (default: ./pattern.dat)");
parser.add_argument("--path-pattern", type = str, default = "", help = "generate target with old pattern and specify the path to find it");
parser.add_argument("--epoch", type = int, default = 400, help = "the number of epochs to run (default: 400)");
args = parser.parse_args();

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_mtdf;

rv.board.config();
rv.pattern.config();

name = [args.path_sample, args.path_occur];
if os.path.exists(name[0]) and os.path.exists(name[1]):
	print("sample already exists");
	sample = rv.mat_brd();
	sample.load(name[0]);
	occurrence = rv.mat_i();
	occurrence.load(name[1]);
else:
	print("generate sample");
	time_begin = time.time();
	occurrence = rv.mat_i();
	sample = rv.sample_gen(args.num_simulate, occurrence);
	time_end = time.time();
	print("time: ", time_end - time_begin);
	sample.save(name[0]);
	occurrence.save(name[1]);

size = sample.geth();
print("sample size: ", size);

name = args.path_target;
if os.path.exists(name):
	print("target already exists");
	target = rv.mat_f();
	target.load(name);
else:
	if os.path.exists(args.path_pattern):
		rv.pattern.config(args.path_pattern);
		mthd = mthd | rv.mthd_ptn;
	print("generate target");
	time_begin = time.time();
	target = rv.evaluate(sample, mthd, 4);
	time_end = time.time();
	print("time: ", time_end - time_begin);
	target.save(name);

ptn_shape = rv.pattern().numpy().shape;
weight = np.zeros(ptn_shape, dtype = np.float64).ravel();
target_np = target.numpy();
#occurrence_np = occurrence.numpy() ** (3 / 4);

def fun(weight):
	global size;
	value = rv.evaluate(rv.pattern(weight.astype(np.float32).reshape(ptn_shape)), sample);
	delta = value.numpy() - target_np;
	loss = (delta ** 2).mean();
	ptn_grad = rv.pattern();
	ptn_grad.initial();
	rv.adjust(ptn_grad, sample, rv.mat_f((delta).astype(np.float32)));
	grad = ptn_grad.numpy().ravel();
	return (loss.astype(np.float64), (grad * 2 / size).astype(np.float64));

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

ptn = rv.pattern(result.x.astype(np.float32).reshape(ptn_shape));
ptn.balance();
ptn.save(args.path_save);
