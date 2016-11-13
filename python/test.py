import reversi as rv

grp = rv.group();
grp.load("../data/data1.dat");
grp.train();

g = rv.game();

g.start();



import reversi as rv

grp = rv.group();
%time for i in range(100):grp.train();