import numpy as np;
import pickle;
import os;

import reversi as rv;
rv.board.config();

data_path = "../data/";

with open(data_path + "Reversi-2018-2.dat", "rb") as fobj:
	sample = np.load(fobj);

data = np.array([], dtype = np.uint64);
data = data.reshape((0,4));

for i in sample:
	data = np.vstack((data, i));

num = 100000;
with open("sample.dat", "wb") as fobj:
	fobj.write(int.to_bytes(num, 4, "little"));
	fobj.write(int.to_bytes(1, 4, "little"));
	for i in range(num):
		fobj.write(int.to_bytes(int(data[i, 0]), 8, "little"));
		fobj.write(int.to_bytes(int(data[i, 1]), 8, "little"));
