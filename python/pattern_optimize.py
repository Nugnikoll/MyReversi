import reversi as rv;
import time;
import matplotlib.pyplot as plt;
import os;
import numpy as np;
from scipy import optimize;

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn;
dir_save = "./";

rv.board.config();
rv.pattern.config();
#rv.group.config("../data/pattern.dat");

ptn = rv.pattern();
ptn.initial();

size = 1000;

name = "sample.dat";
if os.path.exists(name):
	sample = rv.mat_brd();
	sample.load(name);
else:
	print("generate sample");
	time1 = time.clock();
	sample = rv.sample_gen(size);
	time2 = time.clock();
	print("time1:",time2 - time1);
	sample = rv.sample_2mat(sample);
	time3 = time.clock();
	print("time2:",time3 - time2);
	sample.save(name);

size = sample.geth();
alpha = 0.004 / size;
beta = 0.001;

print("sample size: ", sample.geth(), sample.getw());

name = "target.dat";
if os.path.exists(name):
	target = rv.mat_lf();
	target.load(name);
else:
	print("generate target");
	time3 = time.clock();
	target = rv.evaluate(sample, mthd & ~rv.mthd_ptn & ~rv.mthd_trans, 4);
	time4 = time.clock();
	print("time3:",time4 - time3);
	target.save(name);

ptn_shape = ptn.numpy().shape;
print("shape:", ptn_shape);
weight = np.zeros(ptn_shape).ravel();

def fun(weight):
	value = rv.evaluate(rv.pattern(weight.reshape(ptn_shape)), sample);
	delta = value - target;
	eps = delta.numpy();
	loss = (eps ** 2).sum();
	ptn_grad = rv.pattern();
	ptn_grad.initial();
	rv.adjust(ptn_grad, sample, delta);
	grad = ptn_grad.numpy().ravel();
	return (loss, grad * 2);

result = optimize.minimize(
	fun, weight, method = "L-BFGS-B", jac = True,
	options = {
		"disp": True,
		"maxcor": 25,
		"maxiter": 400
	}
)

print(result);
