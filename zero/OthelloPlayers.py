import numpy as np

sys.path.append("../python/");
import reversi as rv;

class RandomPlayer():
	def play(self, brd):
		a = np.random.randint(rv.board.size2)
		brd_move = brd.get_move(True);
		while not (brd_move & (1 << a)):
			a = np.random.randint(rv.board.size2)
		return a


class HumanOthelloPlayer():
	def play(self, brd):
		# display(board)
		brd_move = brd.get_move(True);
		for i in range(rv.board.size2):
			if brd_move & (1 << i):
				print(i & 0x7, i >> 3);
		while True:
			a = input()

			x, y = [int(x) for x in a.split(' ')]
			a = x + (y << 3);
			if brd_move & (1 << a):
				break;
			else:
				print('Invalid move');

		return a;


class GreedyOthelloPlayer():
	def play(self, brd):
		brd_move = brd.get_move(True);
		candidates = []
		for a in range(rv.board.size2):
			if not (brd_move & (1 << a)):
				continue;
			nextBoard = rv.board(brd);
			nextBoard.flip(True, a);
			score = nextBoard.score(True);
			candidates += [(-score, a)]
		candidates.sort()
		return candidates[0][1]
