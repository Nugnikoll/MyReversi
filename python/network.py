from __future__ import print_function;
import os;
import time;
import numpy as np;
import pickle;
import torch;
import torch.nn as nn;
import torch.nn.functional as fun;
import torch.optim as optim;
from torchvision import datasets, transforms;
from torch.autograd import Variable;
import argparse;

from display import *;

class network(nn.Module):
	def __init__(self):
		super(network, self).__init__();
		self.relu = nn.ReLU(inplace = True);
		self.conv1 = nn.Conv2d(2, 32, 5, padding = 1);
		self.bn1 = nn.BatchNorm2d(32);
		self.conv2 = nn.Conv2d(32, 64, 3);
		self.bn2 = nn.BatchNorm2d(64);
		self.fc1 = nn.Linear(64 * 4 * 4, 16);
		self.bn3 = nn.BatchNorm1d(16);
		self.fc2 = nn.Linear(16, 1);

	def initial(self):
		nn.init.orthogonal_(self.conv1.weight.data, 0.001);
		self.conv1.bias.data.fill_(0);
		nn.init.orthogonal_(self.conv2.weight.data, 0.001);
		self.conv2.bias.data.fill_(0);
		nn.init.orthogonal_(self.fc1.weight.data, 0.001);
		self.fc1.bias.data.fill_(0);
		nn.init.orthogonal_(self.fc2.weight.data, 0.001);
		self.fc2.bias.data.fill_(0);

	def forward(self, data):
		x = self.conv1(data);
		x = self.bn1(x);
		x = self.relu(x);
		x = self.conv2(x);
		x = self.bn2(x);
		x = self.relu(x);
		x = x.view(-1, 64 * 4 * 4);
		x = self.fc1(x);
		x = self.bn3(x);
		x = self.relu(x);
		x = self.fc2(x);
		return x;

	def cost(self, data, target):
		result = self.__call__(data);
		loss = ((result - target) ** 2).sum() / data.shape[0];
		return loss;

#class network(nn.Module):
#	def __init__(self):
#		super(network, self).__init__();
#		self.relu = nn.ReLU(inplace = True);
#		self.conv1 = nn.Conv2d(2, 16, 5, padding = 1);
#		self.pool1 = nn.MaxPool2d(2);
#		self.bn1 = nn.BatchNorm2d(16);
#		self.fc1 = nn.Linear(16 * 3 * 3, 16);
#		self.bn2 = nn.BatchNorm1d(16);
#		self.fc2 = nn.Linear(16, 1);

#	def forward(self, data):
#		x = self.conv1(data);
#		x = self.pool1(x);
#		x = self.bn1(x);
#		x = self.relu(x);
#		x = x.view(-1, 16 * 3 * 3);
#		x = self.fc1(x);
#		x = self.bn2(x);
#		x = self.relu(x);
#		x = self.fc2(x);
#		return x;

#	def cost(self, data, target):
#		result = self.__call__(data);
#		loss = ((result - target) ** 2).sum() / data.shape[0];
#		return loss;

