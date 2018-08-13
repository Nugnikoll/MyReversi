import reversi as rv;

rv.board.config();
rv.pattern.config();
ptn = rv.pattern();
ptn.load("pattern.dat");
ptn.balance();
ptn.save("pattern_balance.dat");
