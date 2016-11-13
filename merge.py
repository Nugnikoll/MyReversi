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
	line = lines[i];
	line = re.match(r".*#include *\"([a-zA-z\._/]*)\" *//\+\+.*",line);
	if line == None:
		continue;
	line = re.sub(r".*#include *\"([a-zA-z\._/]*)\" *//\+\+.*",r"\g<1>",line.group(0));
	print("include file : %s" % line);
	if not os.path.isfile(line):
		print("Cannot find the file: %s" % line);
		exit(1);
	fobj = open(line);
	lines[i] = fobj.read();
	fobj.close();

for i in range(1,len(lines)):
	lines[i] = re.sub(r"[^\n]*#include *\"([a-zA-z\._/]*)\" *//--[^\n]*","",lines[i]);

filename = sys.argv[2];

fobj = open(filename,"w");
lines = fobj.writelines(lines);
fobj.close();