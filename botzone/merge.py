import re;
import os;
import codecs

src_dir = "../cpp/";

src = ["type.h","asm.h","reversi.h", "hash.h","matrix.h","pattern.h",
	"reversi.cc","flip.cc","search.cc","pattern.cc"];
src = [src_dir + x for x in src] + ["main.cc"];

lines = [];
for filename in src:
	assert(os.path.isfile(filename))
	fobj = codecs.open(filename,"r","utf-8");
	lines += fobj.readlines();
	lines[-1] += "\n";
	fobj.close();

lines = "".join(lines);

# suppress redundant include header macros
lines = re.sub(r"\s*#include *\"(?!json).*\".*\n","\n",lines);

target = "main_merge.cc"

fobj = codecs.open(target,"w","utf-8");
fobj.writelines(lines);
fobj.close();
