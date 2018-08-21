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

net = stacked_autoencoder((128, 64, 32, 16), 1, 0.003, 0.1, 3, 0.0001);

with open("train.dat", "rb") as fobj:
	net.state_dict(pickle.load(fobj));

m = rv.mat_f(net.fc0.weight.t().data.numpy().copy());
m.save("fc0_w.dat");
m = rv.mat_f(net.fc0.bias.reshape(1, -1).data.numpy());
m.save("fc0_b.dat");
m = rv.mat_f(net.fc1.weight.t().data.numpy().copy());
m.save("fc1_w.dat");
m = rv.mat_f(net.fc1.bias.reshape(1, -1).data.numpy());
m.save("fc1_b.dat");
m = rv.mat_f(net.fc2.weight.t().data.numpy().copy());
m.save("fc2_w.dat");
m = rv.mat_f(net.fc2.bias.reshape(1, -1).data.numpy());
m.save("fc2_b.dat");
m = rv.mat_f(net.theta.weight.t().data.numpy().copy());
m.save("theta.dat");
