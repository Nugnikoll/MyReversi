import re;
import os;
import codecs

filename = "main_merge.cc";

fobj = codecs.open(filename,"r","utf-8");
lines = fobj.readlines();
fobj.close();

lines = "".join(lines);

# suppress single line comments
lines = re.sub(r"(.*)//.*\n","\g<1>\n",lines);
# suppress multi-line comments
lines = re.sub(r"\/\*[^\/]*\*\/","",lines);
# suppress newline
lines = re.sub(r"\\\s*\n","",lines);

# suppress blank
lines = re.sub(r"\s*\n\s*","\n",lines);
lines = re.sub(r"(?<=\n)(?!\s*#)([^\n]*)\n(?!\s*#)","\g<1>",lines);

lines_buff = [];
while True:
	piece = re.search(r"[^\"']*(?:[\"'])",lines);
	if piece == None:
		break;
	piece = piece.span();
	lines_buff += [lines[piece[0]:piece[1]]];
	lines = lines[piece[1]:];
lines = lines_buff + [lines];

for i in range(len(lines)):
	if (i & 1) == 0:
		lines[i] = re.sub(r"(?:\B(?:(?!\n)\s)+\B|\b(?:(?!\n)\s)+\B|\B(?:(?!\n)\s)+\b)","",lines[i]);
lines = "".join(lines);

# replace variable names
lines = re.sub(r"\bboard\b","b",lines);
lines = re.sub(r"\bcolor\b","cl",lines);
lines = re.sub(r"\bchoice\b","ch",lines);
lines = re.sub(r"\bcoordinate\b","co",lines);
lines = re.sub(r"\bchessman\b","cm",lines);
lines = re.sub(r"\bgame\b","g",lines);
lines = re.sub(r"\bpattern\b","pn",lines);
lines = re.sub(r"\bbrd_blue\b","b_b",lines);
lines = re.sub(r"\bbrd_green\b","b_g",lines);
lines = re.sub(r"\bbrd_white\b","b_w",lines);
lines = re.sub(r"\bbrd_black\b","b_bl",lines);
lines = re.sub(r"\bbrd_type\b","b_t",lines);
lines = re.sub(r"\bcbrd_type\b","cb_t",lines);
lines = re.sub(r"\bpos_type\b","p_t",lines);
lines = re.sub(r"\bcpos_type\b","cp_t",lines)
lines = re.sub(r"\bcalc_type\b","c_t",lines);
lines = re.sub(r"\bccalc_type\b","cc_t",lines);
lines = re.sub(r"\bheight\b","h",lines);
lines = re.sub(r"\balpha\b","A",lines);
lines = re.sub(r"\bbeta\b","B",lines);
lines = re.sub(r"\bgamma\b","G",lines);
lines = re.sub(r"\bmask\b","m",lines);
lines = re.sub(r"\temp\b","t",lines);

filename = "main_compress.cc";

fobj = codecs.open(filename,"w");
fobj.writelines(lines);
fobj.close();