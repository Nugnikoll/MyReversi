import os;
import sys;
import time;
import signal as sig;
import json;
import argparse;

sys.path.append("../python");
import reversi as rv;

rv.board.config();
rv.pattern.config();

parser = argparse.ArgumentParser(description = "tornament of two reversi programs");
parser.add_argument("--num", type = int, default = 100, help = "number of matches to play (default: 100)");
parser.add_argument("--path1", type = str, required = True, help = "path to find the first program");
parser.add_argument("--path2", type = str, default = None, help = "path to find the second program");
args = parser.parse_args();

if args.path2 is None:
	args.path2 = args.path1;

class process:
	def __init__(self, path):
		self.path = path;
		self.pipe1 = os.pipe();
		self.pipe2 = os.pipe();
		self.pid = os.fork();
		if self.pid < 0:
			print("Failed to fork a child", file = sys.stderr);
			exit();
		elif self.pid:
			self.fobj1 = os.fdopen(self.pipe1[1], "w");
			self.fobj2 = os.fdopen(self.pipe2[0], "r");
			os.close(self.pipe1[0]);
			os.close(self.pipe2[1]);
		else:
			os.dup2(self.pipe1[0], sys.stdin.fileno());
			os.dup2(self.pipe2[1], sys.stdout.fileno());
			os.close(self.pipe1[0]);
			os.close(self.pipe2[1]);
			os.close(self.pipe1[1]);
			os.close(self.pipe2[0]);
			path_list = self.path.split(" ");
			os.execl(path_list[0], *path_list);
			print("Failed to execl a program", file = sys.stderr);
			exit();

	def communicate(self, msg):
		self.fobj1.write(msg);
		self.fobj1.flush();
		return self.fobj2.readline();

proc = [];
proc.append(process(args.path1));
proc.append(process(args.path2));

score = [[0, 0, 0], [0, 0, 0]];

half = args.num // 2;
flag_switch = False;

for i in range(args.num):
	brd = rv.board();
	color = True;
	brd.initial();
	while True:
		request = {
			"request": {
				"color": color,
				"board": {
					"black": brd.get_brd(True),
					"white": brd.get_brd(False)
				}
			}
		};
		msg = json.dumps(request);
		#print(msg);
		msg = proc[color ^ flag_switch].communicate(msg + "\n");
		#print(msg, end = "");
		response = json.loads(msg);

		x = response["response"]["x"];
		y = response["response"]["y"];
		brd.flip(color, x | (y << 3));
		#brd._print();
		if not brd.get_move(not color):
			if not brd.get_move(color):
				break;
		else:
			color = not color;

	diff = brd.count(True) - brd.count(False);
	print("i=%d, switch=%s, diff=%d" % (i, "True" if flag_switch else "False", diff));
	if diff < 0:
		score[flag_switch][0] += 1;
	elif diff:
		score[flag_switch][2] += 1;
	else:
		score[flag_switch][1] += 1;

	if i == half:
		flag_switch = not flag_switch;

print("proc1=" + proc[0].path);
print("proc2=" + proc[1].path);
print("white=proc1\tblack=proc2\t%d:%d:%d" % (score[0][0], score[0][1], score[0][2]));
print("black=proc2\twhite=proc1\t%d:%d:%d" % (score[1][0], score[1][1], score[1][2]));
