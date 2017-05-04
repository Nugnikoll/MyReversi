import reversi as rv

grp = rv.group();
grp.assign(10);

for i in range(1000):
	grp.train();

grp.save("ptn_d0g10i1000.dat");

print("ptn_d0g10i1000");

for i in range(10000):
	grp.train();

grp.save("ptn_d0g10i10000.dat");

print("ptn_d0g10i10000");