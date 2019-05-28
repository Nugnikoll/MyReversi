import math;
import sys;
import numpy as np;
EPS = 1e-8;

sys.path.append("../python/");
import reversi as rv;

class MCTS():
	"""
	This class handles the MCTS tree.
	"""

	def __init__(self, nnet, args):
		self.nnet = nnet;
		self.args = args;
		self.Qsa = {}	   # stores Q values for s,a (as defined in the paper)
		self.Nsa = {}	   # stores #times edge s,a was visited
		self.Ns = {}		# stores #times board s was visited
		self.Ps = {}		# stores initial policy (returned by neural net)

		self.Es = {}		# stores game.getGameEnded ended for board s
		self.Vs = {}		# stores game.getValidMoves for board s

	def getActionProb(self, brd, temp = 1):
		"""
		This function performs numMCTSSims simulations of MCTS starting from
		brd.

		Returns:
			probs: a policy vector where the probability of the ith action is
				   proportional to Nsa[(s,a)]**(1./temp)
		"""
		for i in range(self.args.numMCTSSims):
			self.search(brd)

		s = brd.view().tostring();
		counts = [self.Nsa[(s,a)] if (s,a) in self.Nsa else 0 for a in range(rv.board.size2)]

		if temp == 0:
			bestA = np.argmax(counts)
			probs = [0] * len(counts)
			probs[bestA]=1
			return probs

		counts = [x**(1. / temp) for x in counts]
		probs = [x / float(sum(counts)) for x in counts]
		return probs


	def search(self, brd):
		"""
		This function performs one iteration of MCTS. It is recursively called
		till a leaf node is found. The action chosen at each node is one that
		has the maximum upper confidence bound as in the paper.

		Once a leaf node is found, the neural network is called to return an
		initial policy P and a value v for the state. This value is propogated
		up the search path. In case the leaf node is a terminal state, the
		outcome is propogated up the search path. The values of Ns, Nsa, Qsa are
		updated.

		NOTE: the return values are the negative of the value of the current
		state. This is done since v is in [-1,1] and if v is the value of a
		state for the current player, then its value is -v for the other player.

		Returns:
			v: the negative of the value of the current canonicalBoard
		"""

		s = brd.view().tostring();

		if s not in self.Es:
			if brd.get_move(True) or brd.get_move(False):
				self.Es[s] = 0;
			else:
				diff = brd.count(True) - brd.count(False);
				if diff > 0:
					self.Es[s] = 1;
				else:
					self.Es[s] = -1;
		if self.Es[s] != 0:
			# terminal node
			return -self.Es[s]

		if s not in self.Ps:
			# leaf node
			self.Ps[s], v = self.nnet.predict(brd.expand());
			brd_move = brd.get_move(True);
			for i in range(brd.size2):
				if not ((1 << i) & brd_move):
					self.Ps[s][i] = 0;
			sum_Ps_s = np.sum(self.Ps[s])
			if sum_Ps_s > 0:
				self.Ps[s] /= sum_Ps_s	# renormalize
			else:
				# if all valid moves were masked make all valid moves equally probable
				
				# NB! All valid moves may be masked if either your NNet architecture is insufficient or you've get overfitting or something else.
				# If you have got dozens or hundreds of these messages you should pay attention to your NNet and/or training process.   
				print("All valid moves were masked, do workaround.")
				self.Ps[s] = self.Ps[s] + valids
				self.Ps[s] /= np.sum(self.Ps[s])

			self.Vs[s] = brd_move
			self.Ns[s] = 0
			return -v

		brd_move = self.Vs[s]
		cur_best = -float('inf')
		best_act = -1

		# pick the action with the highest upper confidence bound
		for a in range(rv.board.size2):
			if brd_move & (1 << a):
				if (s,a) in self.Qsa:
					u = self.Qsa[(s,a)] + self.args.cpuct*self.Ps[s][a]*math.sqrt(self.Ns[s])/(1+self.Nsa[(s,a)])
				else:
					u = self.args.cpuct*self.Ps[s][a]*math.sqrt(self.Ns[s] + EPS)	 # Q = 0 ?

				if u > cur_best:
					cur_best = u
					best_act = a

		a = best_act
		next_s = rv.board(brd);
		next_s.flip(True, a);
		if next_s.get_move(False):
			next_s.reverse();

		v = self.search(next_s)

		if (s,a) in self.Qsa:
			self.Qsa[(s,a)] = (self.Nsa[(s,a)]*self.Qsa[(s,a)] + v)/(self.Nsa[(s,a)]+1)
			self.Nsa[(s,a)] += 1

		else:
			self.Qsa[(s,a)] = v
			self.Nsa[(s,a)] = 1

		self.Ns[s] += 1
		return -v
