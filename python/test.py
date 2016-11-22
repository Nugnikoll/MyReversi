import reversi as rv

grp = rv.group();
grp.assign(2);

for i in range(10):
	for j in range(1000):
		grp.train();
	rv.set_ptn(grp.vec[0]);