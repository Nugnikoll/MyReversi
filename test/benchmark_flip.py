import os
import sys
import time
import numpy as np

sys.path.append("../python")

import reversi as rv

class time_span:
	def __init__(self):
		pass

	def __enter__(self):
		self.t = time.time()
		pass

	def __exit__(self, type, val, traceback):
		t = time.time() - self.t
		print("time: %f" % t)
		pass

rv.board.config()
sample_path = "./sample_pos.npy"
if os.path.exists(sample_path):
	sample = np.load(sample_path)
else:
	sample = rv.sample_pos_gen(10000)
	np.save(sample_path, sample)

with time_span():
	result = rv.benchmark_flip(sample)

diff = (sample[:,3:5] != result)
print("diff: %d" % np.sum(diff))

if diff.any():
	for i in range(diff.shape[0]):
		if diff[i,0] or diff[i,1]:
			brd = rv.board(int(sample[i,1]), int(sample[i,0]))
			print(hex(brd.get_brd(False)), hex(brd.get_brd(True)), int(sample[i, 2]))
			brd._print()
			brd = rv.board(int(sample[i,4]), int(sample[i,3]))
			brd._print()
			brd = rv.board(int(result[i,1]), int(result[i,0]))
			brd._print()
			break;
