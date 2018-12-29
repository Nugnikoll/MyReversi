import reversi as rv;
import struct

class empty:
	pass;

def load_node(fobj):
	ptr = empty();
	brd_white = int.from_bytes(fobj.read(8), "little");
	brd_black = int.from_bytes(fobj.read(8), "little");
	ptr.brd = rv.board(brd_black, brd_white);
	ptr.color = int.from_bytes(fobj.read(1), "little");
	fobj.read(1);
	ptr.height = int.from_bytes(fobj.read(2), "little");
	ptr.depth = int.from_bytes(fobj.read(2), "little");
	fobj.read(2);
	ptr.alpha = struct.unpack('f', fobj.read(4))[0];
	ptr.beta = struct.unpack('f', fobj.read(4))[0];
	ptr.result = struct.unpack('f', fobj.read(4))[0];
	fobj.read(4);
	ptr.child = [];
	return ptr;

def load_log(filename):
	log = empty();
	table = [None for i in range(64)];

	with open(filename, "rb") as fobj:
		log.root = empty();
		log.root.child = [];
		table[0] = log.root;
		while fobj.peek():
			try:
				ptr = load_node(fobj);
				table[ptr.height].child.append(ptr);
				ptr.parent = table[ptr.height];
				table[ptr.height + 1] = ptr;
			except:
				print(ptr.height, ptr.depth, ptr.alpha, ptr.beta, ptr.result);
				break;
	return log;
