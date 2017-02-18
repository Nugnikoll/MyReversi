import reversi as rv

grp = rv.group();
grp.assign(10);

for i in range(1000):
	grp.train();

grp.save("pattern_g10_i1000_2.dat");

print("pattern 1000");

for i in range(10000):
	grp.train();

grp.save("pattern_g10_i10000_2.dat");

print("pattern 10000");