import sys
import struct

sys.path.append("../python")
import reversi as rv

class tree_log:
	pass

class node_log:
	def __init__(self):
		self.parent = None
		self.sibling = None
		self.child = None
	def info(self):
		if self.parent is None:
			return "root"
		else:
			return (
				"color: %d, depth: %d, alpha: %f, beta: %f, result: %f"
				% (self.color, self.depth, self.alpha, self.beta, self.result)
			)

def load_node(fobj):
	ptr = node_log()
	brd_white = int.from_bytes(fobj.read(8), "little")
	brd_black = int.from_bytes(fobj.read(8), "little")
	ptr.brd = rv.board(brd_white, brd_black)
	ptr.color = int.from_bytes(fobj.read(1), "little")
	fobj.read(1)
	ptr.height = int.from_bytes(fobj.read(2), "little")
	ptr.depth = int.from_bytes(fobj.read(2), "little")
	fobj.read(2)
	ptr.alpha = struct.unpack('f', fobj.read(4))[0]
	ptr.beta = struct.unpack('f', fobj.read(4))[0]
	ptr.result = struct.unpack('f', fobj.read(4))[0]
	fobj.read(4)
	ptr.child = []
	return ptr

def load_log(filename):
	log = tree_log()
	table = [None for i in range(65)]
	tree_log.root = node_log()

	with open(filename, "rb") as fobj:
		while fobj.peek():
			try:
				ptr = load_node(fobj)
				if ptr.height:
					ptr.parent = table[ptr.height - 1]
				else:
					ptr.parent = tree_log.root
				ptr.parent.child = ptr
				if not (table[ptr.height] is None) and table[ptr.height].parent == ptr.parent:
					ptr.sibling = table[ptr.height]
				table[ptr.height] = ptr
			except:
				print(ptr.height, ptr.depth, ptr.alpha, ptr.beta, ptr.result)
				break

	return log
