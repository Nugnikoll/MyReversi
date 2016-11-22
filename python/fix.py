import sys;
import re;
import os;

if len(sys.argv) < 3:
	exit(1);

filename = sys.argv[1];
if not os.path.isfile(filename):
	exit(1);

fobj = open(filename);
lines = fobj.readlines();
fobj.close();

for i in range(1,len(lines)):
	lines[i] = lines[i].replace(
		"for (; jj < (size_t)pattern::size1; ++jj) arg1->table1[ii][jj] = arg2[ii][jj];",
		"for (; jj < (size_t)pattern::size1; ++jj){"
			"for (size_t kk = 0; kk < (size_t)pattern::length; ++kk){"
				"arg1->table1[ii][jj][kk] = arg2[ii][jj][kk];"
			"}"
		"}");

filename = sys.argv[2];

fobj = open(filename,"w");
fobj.writelines(lines);
fobj.close();