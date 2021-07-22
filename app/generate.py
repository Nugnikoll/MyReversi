import os
import sys
import argparse
import numpy as np
import signal

import util
import data

sys.path.append("../python")
import reversi as rv

parser = argparse.ArgumentParser(
	description = "generate training samples for reversi program"
)
parser.add_argument(
	"--num-simulate", type = int, default = 10000,
	help = "number of random self-play games (default: 10000)"
)
parser.add_argument(
	"--path-train", type = str, default = "./train.dat",
	help = "path to save training data (default: ./train.dat)"
)
parser.add_argument(
	"--path-pattern", type = str, default = "",
	help = "generate target with old pattern and specify the path to find it"
)
parser.add_argument(
	"--search-depth", type = int, default = 4,
	help = "the depth to search to evaluate each sample (default: 4)"
)
parser.add_argument(
	"--play-depth", type = int, default = 0,
	help = "evaluate sample by costly play-out instead of a single search (default: 0)"
)
parser.add_argument(
	"--sample-alias", type = str, default = "sample",
	help = "alias of sample"
)
parser.add_argument(
	"--target-alias", type = str, default = "target",
	help = "alias of target"
)
parser.add_argument(
	"--play-cnt-alias", type = str, default = "play_cnt",
	help = "alias of play_cnt"
)
parser.add_argument(
	"--play-score-alias", type = str, default = "play_score",
	help = "alias of play_score"
)
args = parser.parse_args()

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_mtdf

rv.board.config()
rv.pattern.config()

name = args.path_train

if not os.path.exists(name):
	head = {"append": []}
	npy_list = []
	sample, target, play_cnt, play_score = None, None, None, None
else:
	with open(name, "rb") as fobj:
		head, (
			sample, target, play_cnt, play_score
		), npy_list = data.load_npy_list(fobj, (
			args.sample_alias,
			args.target_alias,
			args.play_cnt_alias,
			args.play_score_alias,
		), need_full = True)
	if not "append" in head:
		head["append"] = []

if not sample is None:
	print("sample exists")
else:
	print("generate sample")
	with util.timespan():
		sample = rv.sample_gen_select(args.num_simulate)
	head["append"].append({
		"name": args.sample_alias,
		"type": "npy",
		"info": {
			"type": "sample",
			"mthd": rv.mthd_rnd,
		},
	})
	npy_list.append(sample)
	with open(name, "wb") as fobj:
		data.dump_npy_list(fobj, head, npy_list)

print("sample shape", sample.shape)

if os.path.exists(args.path_pattern):
	rv.pattern.config(args.path_pattern)
	mthd = mthd | rv.mthd_ptn

if args.search_depth:
	if not target is None:
		print("target exists")
	else:
		print("generate target")
		with util.timespan():
			rv.board.clear_hash()
			target = rv.evaluate(sample, mthd, args.search_depth, True)

		head["append"].append({
			"name": args.target_alias,
			"type": "npy",
			"info": {
				"type": "target",
				"mthd": mthd,
				"depth": args.search_depth,
				"sample": args.sample_alias,
			},
		})
		npy_list.append(target)
		with open(name, "wb") as fobj:
			data.dump_npy_list(fobj, head, npy_list)

	print("target shape", target.shape)

flag_int = False
def on_interrupt(signum, frame):
	global flag_int
	flag_int = True

if args.play_depth:
	size = sample.shape[0]
	if not play_cnt is None:
		print("play_cnt exists")
	else:
		play_cnt = np.zeros((size,), dtype = np.int32)
		head["append"].append({
			"name": args.play_cnt_alias,
			"type": "npy",
			"info": {
				"type": "play_cnt",
				"play_score": args.play_score_alias,
			},
		})
		npy_list.append(play_cnt)

	if not play_score is None:
		print("play_score exists")
	else:
		play_score = np.zeros((size,), dtype = np.float32)
		head["append"].append({
			"name": args.play_score_alias,
			"type": "npy",
			"info": {
				"type": "play_score",
				"mthd": mthd,
				"depth": args.play_depth,
				"sample": args.sample_alias,
				"play_cnt": args.play_cnt_alias,
			},
		})
		npy_list.append(play_score)

	max_cnt = play_cnt.max()
	min_cnt = play_cnt.min()
	if max_cnt == min_cnt:
		max_cnt += 1

	with util.timespan():
		rv.board.clear_hash()
		handler_save = signal.signal(signal.SIGINT, on_interrupt)
		for i in range(size):
			if flag_int:
				break
			cnt = play_cnt[i]
			for j in range(cnt, max_cnt):
				if flag_int:
					break
				print("i:", i)
				brd = rv.board(int(sample[i, 0]), int(sample[i, 1]))
				brd._print()
				brd.play_out(mthd, True, args.play_depth)
				brd._print()
				score = brd.score_end(True)
				print("score:", score)
				print()
				play_score[i] = (play_score[i] * cnt + score) / (cnt + 1)
				cnt = cnt + 1
			play_cnt[i] = cnt

	with open(name, "wb") as fobj:
		data.dump_npy_list(fobj, head, npy_list)

	signal.signal(signal.SIGINT, handler_save)
