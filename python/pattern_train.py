import reversi as rv

brd = rv.board();
brd.config();

grp = rv.group();

mthd = input("method : ");
mthd_full = "mthd_" + mthd
mthd_id = getattr(rv,mthd_full)
depth = input("depth : ");
size = input("group size : ");
num = input("number : ");

grp.assign(size);
grp.initial();

for i in range(num):
	grp.train(mthd_id,depth);

name = "ptn_%s_d%ds%dn%d" % (mthd,depth,size,num)

grp.save(name + ".dat");

print(name);