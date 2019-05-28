import numpy as np
from progress.bar import Bar
from utils import AverageMeter
import time
import sys;

sys.path.append("../python/");
import reversi as rv;

class Arena():
	"""
	An Arena class where any 2 agents can be pit against each other.
	"""
	def __init__(self, player1, player2):
		"""
		Input:
			player 1,2: two functions that takes board as input, return action
			game: Game object
			display: a function that takes board as input and prints it (e.g.
					 display in othello/OthelloGame). Is necessary for verbose
					 mode.

		see othello/OthelloPlayers.py for an example. See pit.py for pitting
		human players/other baselines with each other.
		"""
		self.player1 = player1
		self.player2 = player2

	def playGame(self, verbose = False):
		"""
		Executes one episode of a game.

		Returns:
			either
				winner: player who won the game (1 if player1, -1 if player2)
			or
				draw result returned from the game that is neither 1, -1, nor 0.
		"""
		players = [self.player2, self.player1];
		curPlayer = True;
		brd = rv.board();
		brd.initial();
		it = 0
		while brd.get_move(True) or brd.get_move(False):
			it += 1
			if verbose:
				print("Turn ", str(it), "Player ", str(curPlayer))
				brd._print();
			action = players[not curPlayer](brd if curPlayer else rv.board(brd).reverse());
			action = int(action);
			brd_move = brd.get_move(curPlayer);

			if not(brd_move & (1 << action)):
				print(action);
				assert False;
			brd.flip(curPlayer, action);
			if brd.get_move(not curPlayer):
				curPlayer = not curPlayer;
		if verbose:
			print("Game over: Turn ", str(it), "Result ", str(self.game.getGameEnded(board, 1)))
			brd._print();

		if brd.get_move(True) or brd.get_move(False):
			r = 0;
		else:
			diff = brd.count(True) - brd.count(False);
			if diff > 0:
				r = 1;
			else:
				r = -1;
		return r;

	def playGames(self, num, verbose = False):
		"""
		Plays num games in which player1 starts num/2 games and player2 starts
		num/2 games.

		Returns:
			oneWon: games won by player1
			twoWon: games won by player2
			draws:  games won by nobody
		"""
		eps_time = AverageMeter()
		bar = Bar('Arena.playGames', max=num)
		end = time.time()
		eps = 0
		maxeps = int(num)

		num = int(num/2)
		oneWon = 0
		twoWon = 0
		draws = 0
		for _ in range(num):
			gameResult = self.playGame(verbose=verbose)
			if gameResult==1:
				oneWon+=1
			elif gameResult==-1:
				twoWon+=1
			else:
				draws+=1
			# bookkeeping + plot progress
			eps += 1
			eps_time.update(time.time() - end)
			end = time.time()
			bar.suffix  = '({eps}/{maxeps}) Eps Time: {et:.3f}s | Total: {total:} | ETA: {eta:}'.format(eps=eps+1, maxeps=maxeps, et=eps_time.avg,
																									   total=bar.elapsed_td, eta=bar.eta_td)
			bar.next()

		self.player1, self.player2 = self.player2, self.player1
		
		for _ in range(num):
			gameResult = self.playGame(verbose=verbose)
			if gameResult==-1:
				oneWon+=1				
			elif gameResult==1:
				twoWon+=1
			else:
				draws+=1
			# bookkeeping + plot progress
			eps += 1
			eps_time.update(time.time() - end)
			end = time.time()
			bar.suffix  = '({eps}/{maxeps}) Eps Time: {et:.3f}s | Total: {total:} | ETA: {eta:}'.format(eps=eps+1, maxeps=num, et=eps_time.avg,
																									   total=bar.elapsed_td, eta=bar.eta_td)
			bar.next()
			
		bar.finish()

		return oneWon, twoWon, draws
