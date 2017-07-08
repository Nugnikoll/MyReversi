import reversi as rv
import time
import matplotlib.pyplot as plt

rv.board.config();
rv.pattern.config();
rv.group.config("ptn_opt.dat");

ptn = rv.pattern();
ptn.initial();

size = 100000;
alpha = 0.004 / size;

time1 = time.clock();
sample = rv.sample_gen(size);
time2 = time.clock();
print("time1",time2 - time1)
index = rv.sample_process(sample);
time3 = time.clock();
print("time2",time3 - time2)
target = rv.evaluate(sample,rv.mthd_default | rv.mthd_ptn,4);
time4 = time.clock();
print("time3",time4 - time3);

index.save("index.dat");
target.save("target.dat");

value = rv.evaluate(ptn,index);
epsilon = target - value;
epsilon_2 = epsilon.modulus();

epsilon_save = rv.mat_f(epsilon);
ptn_save = rv.pattern(ptn)

epsilon = rv.mat_f(epsilon_save);
ptn = rv.pattern(ptn_save);

print("alpha: ",alpha);
print("epsilon_2:",epsilon_2);

for i in range(20000):
	epsilon *= alpha;
	rv.adjust(ptn,index, epsilon);

	value = rv.evaluate(ptn,index);
	epsilon = target - value;
	if i % 10 == 0:
		epsilon_2 = epsilon.modulus();
		print("i: ",i," epsilon_2: ",epsilon_2);

grp = rv.group();
grp.vec.append(ptn);
grp.save("ptn_opt.dat");
