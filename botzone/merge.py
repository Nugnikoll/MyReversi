import re
import os
import codecs

src_dir = "../cpp/"

src = ["type.h", "asm.h", "board.h", "hash.h", "pattern.h",
	"board.cc", "flip.cc", "search.cc","pattern.cc"]
src = [src_dir + x for x in src] + ["main.cc"]

text = ""
for filename in src:
	assert(os.path.isfile(filename))
	with codecs.open(filename, "r", "utf-8") as fobj:
		text += fobj.read() + "\n"

# suppress redundant macros
text = re.sub(r"\s*#include *\"(?![a-zA-Z/.]*json).*\".*\n", "\n", text)

target = "main_merge.cc"

with codecs.open(target, "w", "utf-8") as fobj:
	fobj.write(text)
