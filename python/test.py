# import reversi as rv

# grp = rv.group();
# grp.assign(2);

# for i in range(10):
	# for j in range(1000):
		# grp.train();
	# rv.set_ptn(grp.vec[0]);

##################################

import reversi as rv
a = rv.pos_array(10);
t = rv.tree();

for i in range(100):
	t.practice(rv.mthd_ab,4);

# t.practice(rv.mthd_ab,4);

# t._print();
# t.save("temp.dat");