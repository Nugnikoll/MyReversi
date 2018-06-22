import numpy as np;
import time;
import pickle;
import json;
import zipfile;
import os;
import signal;
import pysigset;

import reversi as rv;
rv.board.config();
rv.pattern.config();
rv.group.config("data/pattern.dat");
mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn;

data_path = "../data/";

flag_int = False;
def on_interrupt(signum, frame):
	global flag_int;
	flag_int = True;

def generate_table(sample, begin = None):
	assert(os.path.exists(data_path + sample));
	with open(data_path + sample, "rb") as fobj:
		sample = pickle.load(fobj);

	if not os.path.exists(data_path + "table.dat"):
		table = np.zeros((65, 32, 32, 4));
		table[:, :, :, 2] = 1;
		with open(data_path + "table.dat", "wb") as fobj:
			pickle.dump([table, [0, 0]], fobj);

	with open(data_path + "table.dat", "rb") as fobj:
		[table, num] = pickle.load(fobj);

	if not (begin is None):
		num = begin;

	handler_save = signal.signal(signal.SIGINT, on_interrupt);

	for i in range(num[0], len(sample)):
		sub_sample = sample[i];

		for ii in range(num[1], sub_sample.shape[0]):
			brd = rv.board();
			brd.assign(int(sub_sample[ii, 1]), int(sub_sample[ii, 0]));
			cnt = brd.sum();

			childid = os.fork();
			if childid > 0:
				time.sleep(4);
				os.kill(childid, signal.SIGUSR1);
				os.wait();
				print("finish", i, ii);
				if flag_int:
					signal.signal(signal.SIGINT, handler_save);
					return;
			else:
				with open(data_path + "table.dat", "rb") as fobj:
					[table, num] = pickle.load(fobj);

				num[1] += 1;
				if num[1] == len(sample[num[0]]):
					num[0] += 1;
					num[1] = 0;

				record = [];
				for j in range(0, 32):
					pmthd = brd.process_method(mthd, j);
					record.append(brd.search(pmthd.first, True, pmthd.second));
					for k in range(0, j):
						l = table[cnt, k, j, 0];
						r = sub_sample[ii, 2];
						bias = record[j] - record[k];
						table[cnt, k, j, 0] = l + r;
						table[cnt, k, j, 2] *= l / (l + r);
						table[cnt, k, j, 2] += abs(bias - table[cnt, k, j, 1]) * r / (l + r);
						table[cnt, k, j, 1] *= l / (l + r);
						table[cnt, k, j, 1] += bias * r / (l + r);
					if j >= 5:
						with pysigset.suspended_signals(signal.SIGUSR1):
							with open(data_path + "table.dat", "wb") as fobj:
								pickle.dump([table, num], fobj);

				signal.signal(signal.SIGINT, handler_save);
				return;

		num[1] = 0;

generate_table("Reversi-2018-2.dat");
