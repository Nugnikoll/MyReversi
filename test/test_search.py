import os
import sys

sys.path.append("../python")
sys.path.append("../app")

import reversi as rv
import data

rv.board.config()
rv.pattern.config("../data/pattern.dat")

def choice2str(self):
	return "(\"%c%c\",%f,%f)" % (
		chr((self.pos & 0x7) + ord("A")),
		chr((self.pos >> 3) + ord("1")),
		self.alpha, self.beta
	)
setattr(rv.choice, "__str__", choice2str)

def vals2str(self):
	return "[" + ",".join([str(v) for v in self]) + "]"
setattr(rv.ints, "__str__", vals2str)
setattr(rv.floats, "__str__", vals2str)
setattr(rv.choices, "__str__", vals2str)

sample_path = "sample.dat"
if os.path.exists(sample_path):
	with open(sample_path, "rb") as fobj:
		(sample,)  = data.load_list(fobj, ("sample",))
else:
	sample = rv.sample_gen_select(1000)
	print(type(sample), sample)
	head = {
		"append":[
			{
				"name": "sample",
				"type": "npy",
				"info": {
					"type": "sample",
					"mthd": rv.mthd_rnd,
				},
			}
		]
	}

	with open(sample_path, "wb") as fobj:
		data.dump_list(fobj, head, [sample])

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn
depth = -2

size = sample.shape[0]
for i in range(size):
	rv.board.clear_hash()
	brd = rv.board(int(sample[i, 0]), int(sample[i, 1]))
	if not brd.get_move(True):
		continue
	brd._print()
	p_mthd = brd.process_method(mthd, depth)
	choices = brd.get_choice(p_mthd.first, True, p_mthd.second)
	print("choices", choices)
	choices = [x for x in choices]
	best = max(choices, key = lambda x:x.alpha)
	print("best", best)
	assert(abs(best.alpha - best.beta) < 1e-3)
	rv.board.clear_hash()
	brd.flip(True, best.pos)
	value = brd.search(p_mthd.first, False, p_mthd.second - 1)
	print("value", value)
	assert(abs(best.alpha + value) < 1e-3)
