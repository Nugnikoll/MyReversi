# -*- coding: utf-8 -*-

import re
import codecs

def wxs2xrc(wxsfile,xrcfile):
	fobj = codecs.open(wxsfile,"r","UTF-8");
	lines = fobj.readlines();
	fobj.close();

	lines_parse = [];
	count = 0;
	for line in lines:
		if re.match(r".*wxFileDialog",line) != None:
			count = 4;
		if count > 0:
			count -= 1;
			continue;
		lines_parse += re.sub(r"wxsmith",r"resource",line)

	fobj = codecs.open(xrcfile,"w","UTF-8");
	fobj.writelines(lines_parse);
	fobj.close();