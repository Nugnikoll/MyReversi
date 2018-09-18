import reversi as rv;
import time;
import matplotlib.pyplot as plt;
import os;
import numpy as np;
from scipy import optimize;

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_mtdf;
dir_save = "./";

rv.board.config();
rv.pattern.config();

name = ["sample.dat", "occurrence.dat"];
if os.path.exists(name[0]) and os.path.exists(name[1]):
	print("sample already exists");
	sample = rv.mat_brd();
	sample.load(name[0]);
	occurrence = rv.mat_i();
	occurrence.load(name[1]);
else:
	size = int(input("Please input number of simulations: "));
	print("generate sample");
	time_begin = time.time();
	occurrence = rv.mat_i();
	sample = rv.sample_gen(size, occurrence);
	time_end = time.time();
	print("time: ", time_end - time_begin);
	sample.save(name[0]);
	occurrence.save(name[1]);

print("sample size: ", sample.geth());

name = "target.dat";
if os.path.exists(name):
	print("target already exists");
	target = rv.mat_lf();
	target.load(name);
else:
	response = "";
	while response != "y" and response != "n":
		response = input("Whether generate target with old pattern? y/[n] ");
	if response == "y":
		rv.pattern.config("./pattern.dat");
		mthd = mthd | rv.mthd_ptn;
	print("generate target");
	time_begin = time.time();
	target = rv.evaluate(sample, mthd, 4);
	time_end = time.time();
	print("time: ", time_end - time_begin);
	target.save(name);

ptn_shape = rv.pattern().numpy().shape;
weight = np.zeros(ptn_shape).ravel();
target_np = target.numpy();
occurrence_np = np.sqrt(occurrence.numpy());

def fun(weight):
	value = rv.evaluate(rv.pattern(weight.reshape(ptn_shape)), sample);
	delta = value.numpy() - target_np;
	loss = (delta ** 2 * occurrence_np).sum();
	ptn_grad = rv.pattern();
	ptn_grad.initial();
	rv.adjust(ptn_grad, sample, rv.mat_lf(delta * occurrence_np));
	grad = ptn_grad.numpy().ravel();
	return (loss, grad * 2);

time_begin = time.time();
result = optimize.minimize(
	fun, weight, method = "L-BFGS-B", jac = True,
	options = {
		"disp": True,
		"maxcor": 25,
		"maxiter": 400
	}
)
time_end = time.time();
print("time: ", time_end - time_begin);

print(result);
ptn = rv.pattern(result.x.reshape(ptn_shape));
ptn.save("pattern.dat");
