import reversi as rv

dir_save = "./";

rv.board.config();
rv.pattern.config();
grp = rv.group();
grp.load("ptn_opt.dat");
grp.at(0).balance();
grp.save("ptn_balance.dat");