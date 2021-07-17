import os
import sys
import time
import json
import shlex
import argparse
import subprocess

sys.path.append("../python")
import reversi as rv

rv.board.config()
rv.pattern.config()

parser = argparse.ArgumentParser(description = "tornament of two reversi programs")
parser.add_argument("--num", type = int, default = 100, help = "number of matches to play (default: 100)")
parser.add_argument("--path1", type = str, required = True, help = "path to find the first program")
parser.add_argument("--path2", type = str, default = None, help = "path to find the second program")
args = parser.parse_args()

if args.path2 is None:
	args.path2 = args.path1

class process:
	def __init__(self, path):
		self.path = path
		self.proc = subprocess.Popen(
			shlex.split(path), stdin = subprocess.PIPE,
			stdout = subprocess.PIPE, stderr = subprocess.PIPE
		)

	def communicate(self, msg):
		if type(msg) is str:
			self.proc.stdin.write(msg.encode("utf-8"))
		else:
			self.proc.stdin.write(msg)
		self.proc.stdin.flush()
		return self.proc.stdout.readline()

proc = []
proc.append(process(args.path1))
proc.append(process(args.path2))

score = [[0, 0, 0], [0, 0, 0]]

half = args.num // 2

for i in range(args.num):
	brd = rv.board()
	color = True
	brd.initial()
	while True:
		request = {
			"request": {
				"color": color,
				"board": {
					"black": brd.get_brd(True),
					"white": brd.get_brd(False)
				}
			}
		}
		flag_switch = (i >= half)

		msg = json.dumps(request)
		#print(msg)
		msg = proc[color ^ flag_switch].communicate(msg + "\n")
		#print(msg, end = "")
		response = json.loads(msg)

		x = response["response"]["x"]
		y = response["response"]["y"]
		brd.flip(color, x | (y << 3))
		#brd._print()
		if not brd.get_move(not color):
			if not brd.get_move(color):
				break
		else:
			color = not color

	diff = brd.count(True) - brd.count(False)
	print("i=%d, switch=%s, diff=%d" % (i, "True" if flag_switch else "False", diff))
	if diff < 0:
		score[flag_switch][0] += 1
	elif diff:
		score[flag_switch][2] += 1
	else:
		score[flag_switch][1] += 1

print("proc1=" + proc[0].path)
print("proc2=" + proc[1].path)
print("white=proc1\tblack=proc2\t%d:%d:%d" % (*score[0],))
print("white=proc2\tblack=proc1\t%d:%d:%d" % (*score[1],))
