import reversi as rv;
import time;
import matplotlib.pyplot as plt;
import os;
import numpy as np;
from network import *;

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
	target = rv.mat_f();
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
sample_np = sample_np.reshape(-1, 2, 8, 8);
target_np = target.numpy();
#sample_ts = torch.Tensor(sample_np);
#target_ts = torch.Tensor(target_np);

def train(epoch, sample_np, target_np, batch_size):
	model.train();
	index = np.array(range(sample_np.shape[0]));
	np.random.shuffle(index);
	sample_np = sample_np[index, :, :];
	target_np = target_np[index, :];
	sample_ts = torch.Tensor(sample_np);
	target_ts = torch.Tensor(target_np);
	for i in range(sample_np.shape[0] // batch_size):
		sample_batch = sample_ts[i * batch_size: (i + 1) * batch_size];
		target_batch = target_ts[i * batch_size: (i + 1) * batch_size];
		optimizer.zero_grad();
		result = model(sample_batch);
		loss = fun.mse_loss(result, target_batch);
		loss.backward();
		optimizer.step();
		if i % 10 == 0:
			print("train epoch: {} [{}/{} ({:.0f}%)]\tloss: {:.6f}".format(
				epoch, i * batch_size, sample_np.shape[0],
				100. * i / (sample_np.shape[0] // batch_size), loss.item()
			));
#			print("result");
#			print(result);
#			print("target");
#			print(target_batch);
#			input();

file_model = "model.pt";
file_optim = "optim.pt";
batch_size = 1000;
model = network();
optimizer = optim.Adam(model.parameters(), lr = 0.001);

if os.path.exists(file_model):
	model.load_state_dict(torch.load(file_model));
else:
	model.initial();
if os.path.exists(file_optim):
	optimizer.load_state_dict(torch.load(file_optim));

for epoch in range(100):
	train(epoch, sample_np, target_np, batch_size);
	torch.save(model.state_dict(), file_model);
	torch.save(optimizer.state_dict(), file_optim)