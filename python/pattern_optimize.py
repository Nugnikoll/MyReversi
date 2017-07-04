import reversi as rv
import matplotlib.pyplot as plt

rv.board.config();

ptn = rv.pattern();
ptn.initial();
sample = rv.sample_gen(100);
index = rv.sample_process(sample);
target = rv.evaluate(sample,rv.mthd_default,4);
corr = rv.mat_i2f(rv.correlate(index,index));
corr_2 = corr * corr;

value = rv.evaluate(ptn,index);
epsilon = target - value;
epsilon_2 = epsilon.modulus();
alpha = (
	rv.mat_2f(epsilon.transpose() * corr * epsilon)
	/ rv.mat_2f(epsilon.transpose() * corr_2 * epsilon)
);

print("alpha ",alpha);

epsilon_save = rv.mat_f(epsilon);
ptn_save = rv.pattern(ptn)

epsilon = rv.mat_f(epsilon_save);
ptn = rv.pattern(ptn_save);

print("epsilon_2 ",epsilon_2);

for i in range(2000):
	epsilon_2 = epsilon.modulus();

	epsilon *= alpha
	rv.adjust(ptn,index, epsilon);

	value = rv.evaluate(ptn,index);
	epsilon = target - value;
	epsilon_2 = epsilon.modulus();
	# epsilon_min = epsilon_2;
	print("epsilon_2 ",epsilon_2);
	alpha = (
		rv.mat_2f(epsilon.transpose() * corr * epsilon)
		/ rv.mat_2f(epsilon.transpose() * corr_2 * epsilon)
	);
	print("alpha ",alpha);

grp = rv.group();
grp.vec.append(ptn);
grp.save("ptn_opt.dat");

# result = [];
# for i in range(100):
	# epsilon = rv.mat_f(epsilon_save);
	# ptn = rv.pattern(ptn_save);
	# alpha = -0.0001 * i;
	# epsilon_2 = epsilon.modulus();
	# epsilon *= alpha * 2
	# rv.adjust(ptn,index, - epsilon);

	# value = rv.evaluate(ptn,index);
	# epsilon = target - value;
	# epsilon_2 = epsilon.modulus();
	# print("epsilon_2 ",epsilon_2);
	# result += [epsilon_2];


# print("result min",min(result));
# print("epsilon min",epsilon_min);
# plt.plot(range(100),result);
# plt.show();