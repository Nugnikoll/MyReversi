import numpy as np;
import math as m;
import matplotlib.pyplot as plt;
import matplotlib.cm as cm;

def display_network(data, colum = None, block = False):
	# This function visualizes filters in matrix A. Each column of A is a
	# filter. We will reshape each column into a square image and visualizes
	# on each cell of the visualization panel. 
	# All other parameters are optional, usually you do not need to worry
	# about it.
	# opt_normalize: whether we need to normalize the filter so that all of
	# them can have similar contrast. Default value is true.
	# opt_graycolor: whether we use gray as the heat map. Default is true.
	# cols: how many columns are there in the display. Default value is the
	# squareroot of the number of columns in A.
	# opt_colmajor: you can switch convention to row major for A. In that
	# case, each row of A is a filter. Default value is false.

	#warning off all

	# rescale
	data -= np.mean(data);

	# compute rows, cols
	(num, size) = data.shape;
	assert(m.sqrt(size) ** 2 == size);
	size = int(m.sqrt(size));

	data = data.reshape(num, size, size);

	if colum == None:
		colum = m.ceil(m.sqrt(num));
		row = m.ceil(num / colum);


	(figure, plotset) = plt.subplots(nrows = row, ncols = colum);

	for i in plotset:
		for j in i:
			j.set_xticks([]);
			j.set_yticks([]);

	for i in range(num):
		r = int(i / row);
		c = i % row;
		image = data[i, :, :];
		image /= np.max(np.abs(image));
		plotset[c, r].imshow(image, cmap = cm.gray);

	plt.show(block = block);

