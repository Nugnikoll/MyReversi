import sys;
sys.path.append("../python/");
import reversi as rv;

rv.board.config();
rv.pattern.config();

from Coach import Coach;
from NNet import NNetWrapper as nn;
from utils import *;

args = dotdict({
	'numIters': 1000,
	'numEps': 100,
	'tempThreshold': 15,
	'updateThreshold': 0.6,
	'maxlenOfQueue': 200000,
	'numMCTSSims': 25,
	'arenaCompare': 40,
	'cpuct': 1,

	'checkpoint': './temp/',
	'load_model': False,
	'load_folder_file': ('/dev/models/8x100x50','best.pth.tar'),
	'numItersForTrainExamplesHistory': 20,
})

if __name__=="__main__":
	nnet = nn();

	if args.load_model:
		nnet.load_checkpoint(args.load_folder_file[0], args.load_folder_file[1])

	c = Coach(nnet, args)
	if args.load_model:
		print("Load trainExamples from file")
		c.loadTrainExamples()
	c.learn()
