import re

filename = "reversi_wrap.cxx";
fobj = open(filename,"r");
lines = fobj.readlines();
fobj.close();

lines = [re.sub(r"std::matrix",r"matrix",line) for line in lines];

fobj = open(filename,"w");
lines = fobj.writelines(lines);
fobj.close();