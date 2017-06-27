import reversi as rv

def train(mthd,depth,size,num):
	brd = rv.board();
	brd.config();

	grp = rv.group();

	mthd_full = "mthd_" + mthd
	mthd_id = getattr(rv,mthd_full)

	grp.assign(size);
	grp.initial();

	for i in range(num):
		grp.train(mthd_id,depth);

	name = "ptn_%s_d%ds%dn%d" % (mthd,depth,size,num)
	grp.save(name + ".dat");
	print(name);