import sys;
from utils import *;

sys.path.append("../python/")
import reversi as rv;

import argparse;
import torch;
import torch.nn as nn;
import torch.nn.functional as F;
import torch.optim as optim;
from torchvision import datasets, transforms;
from torch.autograd import Variable;

class OthelloNNet(nn.Module):
	def __init__(self, args):
		# game params
		self.action_size = rv.board.size2;
		self.args = args

		super(OthelloNNet, self).__init__()
		self.conv1 = nn.Conv2d(2, args.num_channels, 3, stride=1, padding=1)
		self.conv2 = nn.Conv2d(args.num_channels, args.num_channels, 3, stride=1, padding=1)
		self.conv3 = nn.Conv2d(args.num_channels, args.num_channels, 3, stride=1)
		self.conv4 = nn.Conv2d(args.num_channels, args.num_channels, 3, stride=1)

		self.bn1 = nn.BatchNorm2d(args.num_channels)
		self.bn2 = nn.BatchNorm2d(args.num_channels)
		self.bn3 = nn.BatchNorm2d(args.num_channels)
		self.bn4 = nn.BatchNorm2d(args.num_channels)

		self.fc1 = nn.Linear(args.num_channels*(rv.board.size -4)*(rv.board.size - 4), 1024)
		self.fc_bn1 = nn.BatchNorm1d(1024)

		self.fc2 = nn.Linear(1024, 512)
		self.fc_bn2 = nn.BatchNorm1d(512)

		self.fc3 = nn.Linear(512, self.action_size)

		self.fc4 = nn.Linear(512, 1)

	def forward(self, s):
		#														   s: batch_size x board_x x board_y
		s = s.view(-1, 2, rv.board.size, rv.board.size)				# batch_size x 1 x board_x x board_y
		s = F.relu(self.bn1(self.conv1(s)))						  # batch_size x num_channels x board_x x board_y
		s = F.relu(self.bn2(self.conv2(s)))						  # batch_size x num_channels x board_x x board_y
		s = F.relu(self.bn3(self.conv3(s)))						  # batch_size x num_channels x (board_x-2) x (board_y-2)
		s = F.relu(self.bn4(self.conv4(s)))						  # batch_size x num_channels x (board_x-4) x (board_y-4)
		s = s.view(-1, self.args.num_channels*(rv.board.size-4)*(rv.board.size-4))

		s = F.dropout(F.relu(self.fc_bn1(self.fc1(s))), p=self.args.dropout, training=self.training)  # batch_size x 1024
		s = F.dropout(F.relu(self.fc_bn2(self.fc2(s))), p=self.args.dropout, training=self.training)  # batch_size x 512

		pi = self.fc3(s)																		 # batch_size x action_size
		v = self.fc4(s)																		  # batch_size x 1

		return F.log_softmax(pi, dim=1), torch.tanh(v)
