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
	result = rv.sample_flip_benchmark(sample)

print(sample[:, 3:5].shape, result.shape)
diff = (np.sum(sample[:,3:5] != result))
print(diff)
assert(diff == 0)
