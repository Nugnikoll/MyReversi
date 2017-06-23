import re;
import os;
import codecs

src_dir = "../";
src = ["type.h","reversi.h","game.h","pattern.h","tree.h",
	"type.cc","reversi.cc","flip.cc","search.cc","pattern.cc","tree.cc"];
src = [src_dir + x for x in src];
src += ["main.cc"];

lines = [];
for filename in src:
	assert(os.path.isfile(filename))
	fobj = codecs.open(filename,"r","utf-8");
	lines += fobj.readlines();
	lines[-1] += "\n";
	fobj.close();

target = "main_merge.cc"

fobj = codecs.open(target,"w","utf-8");
fobj.writelines(lines);
fobj.close();