import reversi as rv;
import time;
import matplotlib.pyplot as plt;
import os;
import numpy as np;
from network import *;

#mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn;

#rv.board.config();
#rv.pattern.config();

file_model = "model.pt";
mygame.model = network();
if os.path.exists(file_model):
	mygame.model.load_state_dict(torch.load(file_model));
mygame.model.eval();

def play_ai(self, mthd = None, color = None, depth = None):

	if mthd is None:
		mthd = self.mthd;
	if color is None:
		color = self.color;
	if depth is None:
		depth = self.depth;
	self.push();
	#(mthd, depth) = self.process_method(mthd, depth);
	#pos = self.brd.play(mthd, color, depth);

	brd_move = self.brd.get_move(color);
	size = rv.board.count(brd_move);
	_print(hex(brd_move), size);
	if size > 0:
		data = np.empty((size, 2, 64));
		choices = np.empty((size, 2));
		count = 0;
		for pos in range(64):
			if (1 << pos) & brd_move:
				brd_next = rv.board(self.brd);
				brd_next.flip(color, pos);
				if color:
					brd_next.reverse();
				brd_mat = brd_next.to_mat();
				data[count, :, :] = brd_mat.numpy();
				choices[count, 0] = pos;
				count += 1;
		data = data.reshape(size, 2, 8, 8);
		with torch.no_grad():
			result = self.model(torch.Tensor(data)).data.numpy();
		choices[:, 1] = - result.reshape((size,));
		_print(choices);

		index = np.argmax(choices[:, 1]);
		best = choices[index, :];
		pos = int(best[0]);
		self.brd.flip(color, pos);
		pos = (pos & 0x7, pos >> 3);
	else:
		pos = (-1, -1);

	#pos = (pos.x, pos.y);
	if pos[0] >= 0:
		self.print_log(
			"place a " + ("black" if color else "white")
			+ " stone at " + chr(pos[0] + ord("A")) + chr(pos[1] + ord("1")) + "\n"
		);
		if not (self.brd.get_status(not color) & rv.sts_again):
			self.color = not color;
		self.pos = pos;
		self.paint();
	else:
		self.print_log(
			("black" if color else "white")
			+ " is unable to move.\n"
		);
		if self.flag_auto_save:
			self.pop();

	return pos;

game.play_ai = play_ai;

#brd = rv.board();
#def play(color):
#	brd_move = brd.get_move(color);
#	size = rv.board.count(brd_move);
#	choices = np.empty(size, 2);
#	count = 0;
#	for pos in range(64):
#		if (1 << pos) & brd_move:
#			brd_next = rv.board(brd);
#			brd_mat = brd_next.to_mat();
#			brd_np = brd_mat.numpy().reshape(1, 2, 8, 8);
#			result = model(torch.Tensor(brd_np));
#			choices[count, 0] = pos;
#			choices[count, 1] = result.item();
#			++count;
#	index = np.argmax(choices[:, 1]);
#	best = choices[index, :];
#	print(best);
#	brd.flip(color, int(best[0]));

#while True:
#	command = input("player first? [y/n]");
#	if command == "y":
#		color = True;
#		break;
#	elif command == "n";
#		play(True);
#		color = False;
#		break;

#while True:
#	while True:
#		command = input("position:");
#		if(
#			len(command == 2)
#			and ord(command[0]) >= ord("a")
#			and ord(command[0]) <= ord("h")
#			and ord(command[1]) >= ord("1")
#			and ord(command[1]) <= ord("8")
#		):
#			y = ord(command[0] - ord("a"));
#			x = ord(command[1] - ord("1"));
#			pos = y * 8 + x;
#			brd_move = brd.get_move(color);
#			if (1 << pos) & brd_move:
#				
