import reversi as rv;
import time;
import matplotlib.pyplot as plt;
import os;

mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn;
dir_save = "./";

rv.board.config();
rv.pattern.config();
rv.group.config("../data/pattern.dat");

ptn = rv.pattern();
ptn.initial();

size = 100000;
alpha = 0.004 / size;

name = "sample.dat";
if os.path.exists(name):
	sample = rv.mat_brd(size, 1);
	sample.load(name);
else:
	print("generate sample");
	time1 = time.clock();
	sample = rv.sample_gen(size);
	time2 = time.clock();
	print("time1",time2 - time1);
	sample = rv.sample_2mat(sample);
	time3 = time.clock();
	print("time2",time3 - time2);
	sample.save(name);

print(sample.geth(),sample.getw());

name = "target.dat";
if os.path.exists(name):
	target = rv.mat_f(size, 1);
	target.load(name);
else:
	print("generate target");
	time3 = time.clock();
	target = rv.evaluate(sample, mthd & ~rv.mthd_ptn & ~rv.mthd_trans ,4);
	time4 = time.clock();
	print("time3",time4 - time3);
	target.save(name);

value = rv.evaluate(ptn,sample);
epsilon = target - value;
epsilon_2 = epsilon.modulus();

print(epsilon.geth(), epsilon.getw())
epsilon_save = rv.mat_f(epsilon);
ptn_save = rv.pattern(ptn)

epsilon = rv.mat_f(epsilon_save);
ptn = rv.pattern(ptn_save);

print("alpha: ",alpha);
print("epsilon_2: ",epsilon_2);

for i in range(2000):
	epsilon *= alpha;
	rv.adjust(ptn,sample,epsilon);

	value = rv.evaluate(ptn,sample);
	epsilon = target - value;
	if i % 10 == 0:
		print(value.modulus());
		epsilon_2 = epsilon.modulus();
		print("i: ",i," epsilon_2: ",epsilon_2);

grp = rv.group();
grp.vec.append(ptn);
grp.save(dir_save + "ptn_opt.dat");
