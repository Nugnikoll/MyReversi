import numpy as np;
import pickle;
import json;
import zipfile;
import os;
import signal;
import pysigset;

import reversi as rv;
rv.board.config();

data_path = "../data/";

def extract_sample(filename):
	result_pack = [];

	package = zipfile.ZipFile(data_path + filename);
	color = None;

	for name in package.namelist():
		record = {};

		package.extract(name, data_path);
		with open(data_path + name, "r") as fobj:
			lines = fobj.readlines();
		os.remove(data_path + name);

		for line in lines:
			data = json.loads(line);

			assert("log" in data);
			log = data["log"];
			brd = rv.board();
			brd.initial();

			for turn_id in range(len(log)):

				try:
					turn = log[turn_id];
					if "0" in turn:
						color = True;
					elif "1" in turn:
						color = False;
					else:
						continue;

					assert(turn_id + 1 < len(log));
					turn_next = log[turn_id + 1];
					if "output" in turn_next and "display" in turn_next["output"] and "err" in turn_next["output"]["display"]:
						break;

					turn = turn["0" if color else "1"].copy();
					assert("verdict" in turn);
					if turn["verdict"] != "OK":
						break;
					assert("response" in turn);
					turn = turn["response"];
					assert("x" in turn and "y" in turn);
					if turn["x"] < 0 or turn["y"] < 0:
						continue;
					pos = turn["x"] + (turn["y"] << 3);
					brd.flip(color, pos);
					element = (brd.bget(color), brd.bget(not color));
					if element in record:
						record[element] += 1;
					else:
						record[element] = 1;
				except:
					print(turn);
					print(log);
					print(name);
					assert(False);

		result = np.empty((len(record), 4), dtype = np.uint64);
		count = 0;	
		for i, j in record.items():
			result[count, :2] = i;
			result[count, 2] = j;
			count += 1;

		result_pack.append(result);
		print("finish " + name)

	with open(data_path + filename[:-4] + ".dat", "wb") as fobj:
		pickle.dump(result_pack, fobj);

flag_int = False;
def on_interrupt(signum, frame):
	global flag_int;
	flag_int = True;

def generate_label(sample):
	assert(os.path.exists(data_path + sample));
	with open(data_path + sample, "rb") as fobj:
		sample = np.load(fobj);

	if not os.path.exists(data_path + "label.dat"):
		label = np.zeros(sample.shape[0]);
		with open(data_path + "label.dat", "wb") as fobj:
			pickle.dump([0, label], fobj);

	with open(data_path + "label.dat", "rb") as fobj:
		[num, label] = pickle.load(fobj);

	brd = rv.board();
	handler_save = signal.signal(signal.SIGINT, on_interrupt);
	for i in range(num, sample.shape[0]):
		brd.assign(int(sample[i, 0]), int(sample[i, 1]));
		label[i] = brd.search(True, 4);
		if i % 1000 == 0:
			print("finish", i)
		if flag_int:
			break;

	with open(data_path + "label.dat", "wb") as fobj:
		pickle.dump([i, label], fobj);
	signal.signal(signal.SIGINT, handler_save);

extract_sample("Reversi-2018-2.zip");
#generate_label("sample-2018-2.dat");
