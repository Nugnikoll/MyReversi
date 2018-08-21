import reversi as rv;
import time;
import matplotlib.pyplot as plt;
import os;
import numpy as np;
from scipy import optimize;
from nn import *;

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn;
dir_save = "./";

rv.board.config();
rv.pattern.config();
#rv.pattern.config("../data/pattern.dat");

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
	print("time:", time_end - time_begin);
	sample.save(name[0]);
	occurrence.save(name[1]);

size = sample.geth();
print("sample size: %d" % size);

name = "target.dat";
if os.path.exists(name):
	print("target already exists");
	target = rv.mat_lf();
	target.load(name);
else:
	print("generate target");
	time_begin = time.time();
	target = rv.evaluate(sample, mthd & ~rv.mthd_ptn & ~rv.mthd_trans, 4);
	time_end = time.time();
	print("time:", time_end - time_begin);
	target.save(name);

sample_np = np.empty((size, 2, 64), dtype = np.int32);
for i in range(size):
	temp = sample[i].to_mat();
	sample_np[i, :, :] = temp.numpy();
sample_np = sample_np.reshape(size, 128);
target_np = target.numpy();
sample_ts = torch.Tensor(sample_np);
target_ts = torch.Tensor(target_np);

net = stacked_autoencoder((128, 64, 32, 16), 1, 0.003, 0.1, 3, 0.0001);
net.initial();
net.stack(sample_ts, target_ts);
net.train(sample_ts, target_ts);
