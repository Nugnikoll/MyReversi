import reversi as rv;

class empty:
	pass;

def load_node(fobj):
	ptr = empty();
	brd_black = int.from_bytes(fobj.read(8), "little");
	brd_white = int.from_bytes(fobj.read(8), "little");
	ptr.brd = rv.board(brd_black, brd_white);
	ptr.color = int.from_bytes(fobj.read(1), "little");
	fobj.read(1);
	ptr.depth = int.from_bytes(fobj.read(2), "little");
	ptr.alpha = rv.int2float(int.from_bytes(fobj.read(4), "little"));
	ptr.beta = rv.int2float(int.from_bytes(fobj.read(4), "little"));
	ptr.result = rv.int2float(int.from_bytes(fobj.read(4), "little"));
	ptr.child = [];
	return ptr;

def load_log(filename):
	log = empty();
	table = [None for i in range(64)];

	with open(filename, "rb") as fobj:
		log.root = load_node(fobj);
		table[log.root.depth] = log.root;
		while fobj.peek():
			ptr = load_node(fobj);
			table[ptr.depth + 1].child.append(ptr);
			ptr.parent = table[ptr.depth + 1];
			table[ptr.depth] = ptr;

	return log;
