import re;
import os;
import codecs

src_dir = "../"
src = ["type.h","reversi.h","game.h","pattern.h","tree.h",
	"type.cc","reversi.cc","flip.cc","search.cc","pattern.cc","tree.cc"];
src = [src_dir + x for x in src];
src += ["main.cc"];

lines = []
for filename in src:
	assert(os.path.isfile(filename))

	fobj = codecs.open(filename,"r","utf-8");
	lines += fobj.readlines();
	lines[-1] += "\n";
	fobj.close();

lines = [line.strip() for line in lines];

lines_parse = "";
flag = True;
for line in lines:
	if re.match(r"\s*//",line) != None:
		line = "\n";
	if re.match(r"(?:\s*#|.*//|.*\\\s*$)",line) != None:
		if not flag:
			line = "\n" + line + "\n";
		else:
			line += "\n";
		flag = True;
	else:
		flag = False;
	if re.match(r"\s*#include *\".*\"",line) == None:
		lines_parse += line;
lines_parse = re.sub(r"\n+",r"\n",lines_parse);

target = "main_merge.cc"

fobj = open(target,"w");
fobj.writelines(lines_parse);
fobj.close();