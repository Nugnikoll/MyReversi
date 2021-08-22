import os
import sys
import time
import numpy as np

sys.path.append("../python")
sys.path.append("../app")

import reversi as rv
import util

rv.board.config()
sample_path = "./sample_pos.npy"
if os.path.exists(sample_path):
	sample = np.load(sample_path)
else:
	sample = rv.sample_pos_gen(10000)
	np.save(sample_path, sample)

with util.timespan():
	result = rv.benchmark_flip(sample)

diff = (sample[:,3:5] != result)
print("diff: %d" % np.sum(diff))

if diff.any():
	for i in range(diff.shape[0]):
		if diff[i,0] or diff[i,1]:
			brd = rv.board(int(sample[i,0]), int(sample[i,1]))
			print(hex(brd.get_brd(False)), hex(brd.get_brd(True)), int(sample[i, 2]))
			brd._print()
			brd = rv.board(int(sample[i,3]), int(sample[i,4]))
			brd._print()
			brd = rv.board(int(result[i,0]), int(result[i,1]))
			brd._print()
			break;
