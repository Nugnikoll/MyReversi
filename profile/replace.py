import sys
import re
import os

if len(sys.argv) < 3:
	exit(1)

filename = sys.argv[1]
if not os.path.isfile(filename):
	exit(1)

fobj = open(filename)
lines = fobj.readlines()
fobj.close()

for i in range(0,len(lines)):
	lines[i] = re.sub(r"\\n([0-9]*)[^0-9]{2}\"",r'\\n\g<1>"',lines[i])
	lines[i] = re.sub(r"label=\"([0-9]*)[^0-9]{2}\"",r'label="\g<1>"',lines[i])

filename = sys.argv[2]

fobj = open(filename,"w")
lines = fobj.writelines(lines)
fobj.close()
