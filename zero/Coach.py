from collections import deque
from Arena import Arena
from MCTS import MCTS
import numpy as np
from progress.bar import Bar
from utils import AverageMeter
import time, os, sys
from pickle import Pickler, Unpickler
from random import shuffle

sys.path.append("../python/");
import reversi as rv;

class Coach():
	"""
	This class executes the self-play + learning. It uses the functions defined
	in Game and NeuralNet. args are specified in main.py.
	"""
	def __init__(self, nnet, args):
		self.nnet = nnet
		self.pnet = self.pnet = self.nnet.__class__()  # the competitor network
		self.args = args
		self.mcts = MCTS(self.nnet, self.args)
		self.trainExamplesHistory = []	# history of examples from args.numItersForTrainExamplesHistory latest iterations
		self.skipFirstSelfPlay = False # can be overriden in loadTrainExamples()

	def executeEpisode(self):
		"""
		This function executes one episode of self-play, starting with player 1.
		As the game is played, each turn is added as a training example to
		trainExamples. The game is played till the game ends. After the game
		ends, the outcome of the game is used to assign values to each example
		in trainExamples.

		It uses a temp=1 if episodeStep < tempThreshold, and thereafter
		uses temp=0.

		Returns:
			trainExamples: a list of examples of the form (canonicalBoard,pi,v)
						   pi is the MCTS informed policy vector, v is +1 if
						   the player eventually won the game, else -1.
		"""
		trainExamples = []
		brd = rv.board();
		brd.initial();
		self.curPlayer = True
		episodeStep = 0

		while True:
			episodeStep += 1
			canonicalBoard = rv.board(brd);
			if not self.curPlayer:
				canonicalBoard.reverse();
			temp = int(episodeStep < self.args.tempThreshold)

			pi = self.mcts.getActionProb(canonicalBoard, temp = temp);

			def getSymmetries(brd, pi):
				# mirror, rotational
				assert(len(pi) == rv.board.size2);
				l = [];
				pi_board = pi.reshape((rv.board.size, rv.board.size));

				for j in [True, False]:
					newB = rv.board(brd);
					newPi = pi_board.copy();
					if j:
						newB.mirror_h();
						newPi = np.fliplr(newPi);
					l += [(newB, list(newPi.ravel()))];

				for j in [True, False]:
					newB = rv.board(brd);
					newB.rotate_l();
					newPi = np.rot90(pi_board, 1);
					if j:
						newB.mirror_h();
						newPi = np.fliplr(newPi);
					l += [(newB, list(newPi.ravel()))];

				for j in [True, False]:
					newB = rv.board(brd);
					newB.reflect();
					newPi = np.rot90(pi_board, 2);
					if j:
						newB.mirror_h();
						newPi = np.fliplr(newPi);
					l += [(newB, list(newPi.ravel()))];

				for j in [True, False]:
					newB = rv.board(brd);
					newB.rotate_r();
					newPi = np.rot90(pi_board, 3);
					if j:
						newB.mirror_h();
						newPi = np.fliplr(newPi);
					l += [(newB, list(newPi.ravel()))];
				return l

			sym = getSymmetries(canonicalBoard, np.array(pi));
			for b, p in sym:
				trainExamples.append([b.numpy(), self.curPlayer, p, None])

			action = np.random.choice(len(pi), p=pi);
			brd.flip(self.curPlayer, action);
			if brd.get_move(not self.curPlayer):
				self.curPlayer = not self.curPlayer;

			if brd.get_move(True) or brd.get_move(False):
				r = 0;
			else:
				diff = brd.count(self.curPlayer) - brd.count(not self.curPlayer);
				if diff > 0:
					r = 1;
				else:
					r = -1;

			if r!=0:
				return [(x[0],x[2],r*((-1)**(x[1] != self.curPlayer))) for x in trainExamples]

	def learn(self):
		"""
		Performs numIters iterations with numEps episodes of self-play in each
		iteration. After every iteration, it retrains neural network with
		examples in trainExamples (which has a maximium length of maxlenofQueue).
		It then pits the new neural network against the old one and accepts it
		only if it wins >= updateThreshold fraction of games.
		"""

		for i in range(1, self.args.numIters+1):
			# bookkeeping
			print('------ITER ' + str(i) + '------')
			# examples of the iteration
			if not self.skipFirstSelfPlay or i>1:
				iterationTrainExamples = deque([], maxlen=self.args.maxlenOfQueue)
	
				eps_time = AverageMeter()
				bar = Bar('Self Play', max=self.args.numEps)
				end = time.time()
	
				for eps in range(self.args.numEps):
					self.mcts = MCTS(self.nnet, self.args)   # reset search tree
					iterationTrainExamples += self.executeEpisode()
	
					# bookkeeping + plot progress
					eps_time.update(time.time() - end)
					end = time.time()
					bar.suffix  = '({eps}/{maxeps}) Eps Time: {et:.3f}s | Total: {total:} | ETA: {eta:}'.format(eps=eps+1, maxeps=self.args.numEps, et=eps_time.avg, total=bar.elapsed_td, eta=bar.eta_td)
					bar.next()
				bar.finish()

				# save the iteration examples to the history 
				self.trainExamplesHistory.append(iterationTrainExamples)
				
			if len(self.trainExamplesHistory) > self.args.numItersForTrainExamplesHistory:
				print("len(trainExamplesHistory) =", len(self.trainExamplesHistory), " => remove the oldest trainExamples")
				self.trainExamplesHistory.pop(0)
			# backup history to a file
			# NB! the examples were collected using the model from the previous iteration, so (i-1)  
			self.saveTrainExamples(i-1)
			
			# shuffle examples before training
			trainExamples = []
			for e in self.trainExamplesHistory:
				trainExamples.extend(e)
			shuffle(trainExamples)

			# training new network, keeping a copy of the old one
			self.nnet.save_checkpoint(folder=self.args.checkpoint, filename='temp.pth.tar')
			self.pnet.load_checkpoint(folder=self.args.checkpoint, filename='temp.pth.tar')
			pmcts = MCTS(self.pnet, self.args)
			
			self.nnet.train(trainExamples)
			nmcts = MCTS(self.nnet, self.args)

			print('PITTING AGAINST PREVIOUS VERSION')
			arena = Arena(lambda x: np.argmax(pmcts.getActionProb(x, temp=0)),
						  lambda x: np.argmax(nmcts.getActionProb(x, temp=0)))
			pwins, nwins, draws = arena.playGames(self.args.arenaCompare)

			print('NEW/PREV WINS : %d / %d ; DRAWS : %d' % (nwins, pwins, draws))
			if pwins+nwins == 0 or float(nwins)/(pwins+nwins) < self.args.updateThreshold:
				print('REJECTING NEW MODEL')
				self.nnet.load_checkpoint(folder=self.args.checkpoint, filename='temp.pth.tar')
			else:
				print('ACCEPTING NEW MODEL')
				self.nnet.save_checkpoint(folder=self.args.checkpoint, filename=self.getCheckpointFile(i))
				self.nnet.save_checkpoint(folder=self.args.checkpoint, filename='best.pth.tar')				

	def getCheckpointFile(self, iteration):
		return 'checkpoint_' + str(iteration) + '.pth.tar'

	def saveTrainExamples(self, iteration):
		folder = self.args.checkpoint
		if not os.path.exists(folder):
			os.makedirs(folder)
		filename = os.path.join(folder, self.getCheckpointFile(iteration)+".examples")
		with open(filename, "wb+") as f:
			Pickler(f).dump(self.trainExamplesHistory)
		f.closed

	def loadTrainExamples(self):
		modelFile = os.path.join(self.args.load_folder_file[0], self.args.load_folder_file[1])
		examplesFile = modelFile+".examples"
		if not os.path.isfile(examplesFile):
			print(examplesFile)
			r = input("File with trainExamples not found. Continue? [y|n]")
			if r != "y":
				sys.exit()
		else:
			print("File with trainExamples found. Read it.")
			with open(examplesFile, "rb") as f:
				self.trainExamplesHistory = Unpickler(f).load()
			f.closed
			# examples based on the model were already collected (loaded)
			self.skipFirstSelfPlay = True
