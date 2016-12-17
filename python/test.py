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

aa = [1,4,6,7,-1]
for i in range(5):
	a[i] = aa[i];
t.add_path(a.cast(),True);

aa = [1,4,8,9,-1]
for i in range(5):
	a[i] = aa[i];
t.add_path(a.cast(),True);

aa = [1,3,6,8,8,123,-1]
for i in range(7):
	a[i] = aa[i];
t.add_path(a.cast(),True);

t._print();
t.save("temp.dat");

tr = rv.tree();
tr._print();
tr.load("temp.dat");
tr._print();