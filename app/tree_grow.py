import os
import sys
import signal
import argparse

sys.path.append("../python")
import reversi as rv

def str2bool(v):
	if v.lower() in ('yes', 'true', 't', 'y', '1'):
		return True
	elif v.lower() in ('no', 'false', 'f', 'n', '0'):
		return False
	else:
		raise argparse.ArgumentTypeError('Boolean value expected.')

parser = argparse.ArgumentParser(description = "generate a Monte Carlo search tree")
parser.add_argument("--depth", type = int, required = True, help = "search depth at each leaves")
parser.add_argument("--log-interval", type = int, default = 10, help = "interval of log information (default: 1)")
parser.add_argument("--save-interval", type = int, default = 1000, help = "interval of auto saving (default: 100)")
parser.add_argument("--path-save", type = str, default = "", help = "path to save result (default: \"./tree_d%%d.dat\" %% depth)")
parser.add_argument("--overwrite", type = str2bool, default=False, help = "whether to overwrite existing file (default: false)")
args = parser.parse_args()

depth = args.depth
if args.path_save == "":
	name = "tree_d%d.dat" % depth
else:
	name = args.path_save
log_interval = args.log_interval
save_interval = args.save_interval

rv.board.config()
rv.pattern.config("../data/pattern.dat")

brd = rv.board()
brd.initial()

root = rv.node(brd, 0)

if os.path.exists(name) and not args.overwrite:
	print("found file \"%s\"" % name)
	root.load(name)

i = root.count

flag_int = False
def on_interrupt(signum, frame):
	global flag_int
	flag_int = True

handler_save = signal.signal(signal.SIGINT, on_interrupt)
while not flag_int:
	i += 1
	root.grow(True, depth)
	if i % log_interval == 0:
		print("i: %d" % i)
	if i % save_interval == 0:
		root.save(name)
signal.signal(signal.SIGINT, handler_save)

root.save(name)
