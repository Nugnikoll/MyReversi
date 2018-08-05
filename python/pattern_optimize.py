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
rv.group.config("../data/pattern.dat");

ptn = rv.pattern();
ptn.initial();

#name = "ptn_opt.dat";
#if os.path.exists(name):
#	grp = rv.group();
#	grp.load(name);
#	ptn = grp.vec[0];
#else:
#	ptn = rv.pattern();
#	ptn.initial();

size = 100000;
#alpha = 0.004 / size;
#alpha = 0.004 / size * 50;

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

print("sample size: ", sample.geth(), sample.getw());

name = "target.dat";
if os.path.exists(name):
	target = rv.mat_f();
	target.load(name);
else:
	print("generate target");
	time3 = time.clock();
	target = rv.evaluate(sample, mthd & ~rv.mthd_ptn & ~rv.mthd_trans, 4);
	time4 = time.clock();
	print("time3:",time4 - time3);
	target.save(name);

print("shape:", ptn.numpy().shape);
weight = ptn.numpy().ravel();
print(weight.shape)
def fun(weight):
	value = rv.evaluate(ptn, sample);
	epsilon = value - target;
	eps = epsilon.numpy().astype(np.float64);
	loss = (eps ** 2).sum();
	#loss = epsilon.modulus();
	ptn_grad = rv.pattern();
	ptn_grad.initial();
	rv.adjust(ptn_grad, sample, epsilon);
	grad = ptn_grad.numpy().astype(np.float64).ravel() * 2;
	return (loss, grad);

#loss0, grad0 = fun(weight);
#print("loss0: ", loss0);
#epsilon = 1e-5;

#grad_delta = grad0.copy();
#for i in range(10):
#	weight[i] += epsilon;
#	loss_delta = fun(weight)[0];
#	print("loss_delta: ", loss_delta);
#	grad_delta[i] = (loss_delta - loss0) / epsilon;
#	weight[i] = 0;

#print(grad0[:10], grad_delta[:10])

result = optimize.minimize(
	fun, weight, method = "L-BFGS-B", jac = True,
	options = {
		"disp": True,
		"maxcor": 25,
		"maxiter": 400
	}
)

print(result);

#value = rv.evaluate(ptn,sample);
#epsilon = target - value;
#epsilon_2 = epsilon.modulus();

#print(epsilon.geth(), epsilon.getw())
#epsilon_save = rv.mat_f(epsilon);
#ptn_save = rv.pattern(ptn)

#epsilon = rv.mat_f(epsilon_save);
#ptn = rv.pattern(ptn_save);

#print("alpha: ",alpha);
#print("epsilon_2: ",epsilon_2);

#for i in range(2000):
#	epsilon *= alpha;
#	rv.adjust(ptn,sample,epsilon);

#	value = rv.evaluate(ptn,sample);
#	epsilon = target - value;
#	if i % 10 == 0:
#		epsilon_2 = epsilon.modulus();
#		print("i: ",i," epsilon_2: ",epsilon_2);

#grp = rv.group();
#grp.vec.append(ptn);
#grp.save(dir_save + "ptn_opt.dat");
