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

def pca(data, epsilon = 1e-5):
	result = data - data.mean(dim = 1, keepdim = True);
	(u, s, vh) = (result.matmul(result.t()) / result.shape[1]).svd();
	result = (1 / (s + epsilon).sqrt()).reshape((data.shape[0], 1)) * u.t().matmul(result);
	return result;

def zca(data, epsilon = 1e-5):
	result = data - data.mean(dim = 1, keepdim = True);
	(u, s, vh) = (result.matmul(result.t()) / result.shape[1]).svd();
	result = u.matmul((1 / (s + epsilon).sqrt()).reshape((data.shape[0], 1)) * u.t().matmul(result));
	return result;

class autoencoder(nn.Module):
	def __init__(self, input_size, hidden_size, decay, alpha, beta):
		super(autoencoder, self).__init__();
		self.fc1 = nn.Linear(input_size, hidden_size);
		self.bias = nn.Parameter(torch.Tensor(input_size,), requires_grad = True);
		self.decay = decay;
		self.alpha = alpha;
		self.beta = beta;

	def initial(self):
		self.fc1.weight.data.normal_(0, 0.001);
		self.fc1.bias.data.fill_(0);
		self.bias.data.fill_(0);

	def forward(self, data):
		active = fun.sigmoid(self.fc1(data));
		self.rho = active.mean(dim = 0);
		result = fun.sigmoid(self.bias.addmm(active, self.fc1.weight));
		return result;

	def cost(self, data):
		result = self.__call__(data);
		loss = ((result - data) ** 2).sum() / data.shape[0];
		loss += self.decay * (self.fc1.weight ** 2).sum();
		penalty = self.alpha * (self.alpha / self.rho).log() + (1 - self.alpha) * ((1 - self.alpha) / (1 - self.rho)).log();
		loss += self.beta * penalty.sum();
		return loss;

	def train(self, data, lr = 1, max_iter = 400):
		optimizer = optim.LBFGS(self.parameters(), lr = lr, max_iter = max_iter);
		start = time.time();
		def closure():
			optimizer.zero_grad();
			loss = self.cost(data);
			loss.backward();
			closure.count += 1;
			duration = time.time() - start;
			print("count: %d, time: %f, loss: %f" % (closure.count, duration, loss.data.item()));
			return loss;
		closure.count = 0;
		optimizer.step(closure);

	def display(self):
		display_network(self.fc1.weight.data.numpy(), block = True);

class stacked_autoencoder(nn.Module):
	def __init__(self, size, size_class, decay, alpha, beta, theta_decay):
		super(stacked_autoencoder, self).__init__();
		self.num = len(size) - 1;
		for i in range(self.num):
			setattr(self, "fc%d" % i, nn.Linear(size[i], size[i + 1]));
		self.theta = nn.Linear(size[-1], size_class, bias = True);
		self.decay = decay;
		self.alpha = alpha;
		self.beta = beta;
		self.theta_decay = theta_decay;

	def fc(self):
		for i in range(self.num):
			yield getattr(self, "fc%d" % i);

	def initial(self):
		for f in self.fc():
			f.weight.data.normal_(0, 0.001);
			f.bias.data.fill_(0);
		self.theta.weight.data.normal_(0, 0.001);

	def forward(self, data):
		result = data;
		for f in self.fc():
			result = fun.sigmoid(f(result));
		result = self.theta(result);
		return result;

	def cost(self, data, label):
		result = self.__call__(data);
		loss = ((result - label) ** 2).mean();
		#loss += self.theta_decay * (self.theta.weight ** 2).sum();
		return loss;

	def stack(self, data, label):
		for i in range(self.num):
			print("=================== layer %d ===================" % i);
			name = "layer%d.dat" % i;
			f = getattr(self, "fc%d" % i);
			net = autoencoder(f.weight.shape[1], f.weight.shape[0], self.decay, self.alpha, self.beta);
			if os.path.exists(name):
				with open(name, "rb") as fobj:
					param = pickle.load(fobj);
				net.load_state_dict(param);
			else:
				net.initial();
				net.train(data);
				with open(name, "wb") as fobj:
					pickle.dump(net.state_dict(), fobj);
			f = net.fc1;
			setattr(self, "fc%d" % i, f);
			data = fun.sigmoid(f(data));
			data = Variable(data.data);

		print("=================== layer output ===================");
		name = "stack.dat";
		if os.path.exists(name):
			with open(name, "rb") as fobj:
				param = pickle.load(fobj);
			self.load_state_dict(param);
		else:
			optimizer = optim.LBFGS(self.theta.parameters(), max_iter = 400);
			start = time.time();
			def closure():
				optimizer.zero_grad();
				result = self.theta(data);
				loss = ((result - label) ** 2).mean();
				#loss += self.theta_decay * (self.theta.weight ** 2).sum();
				loss.backward();
				closure.count += 1;
				duration = time.time() - start;
				print("count: %d, time: %f, loss: %f" % (closure.count, duration, loss.data.item()));
				return loss;
			closure.count = 0;
			optimizer.step(closure);
			with open(name, "wb") as fobj:
				pickle.dump(self.state_dict(), fobj);

	def train(self, data, label):
		name = "train.dat";
		if os.path.exists(name):
			with open(name, "rb") as fobj:
				param = pickle.load(fobj);
			self.load_state_dict(param);
		else:
			optimizer = optim.LBFGS(self.parameters(), max_iter = 400);
			start = time.time();
			def closure():
				optimizer.zero_grad();
				loss = self.cost(data, label);
				#for f in self.fc:
				#	loss += self.decay * (f.weight ** 2).sum();
				#loss += self.theta_decay * (self.theta.weight ** 2).sum();
				loss.backward();
				closure.count += 1;
				duration = time.time() - start;
				print("count: %d, time: %f, loss: %f" % (closure.count, duration, loss.data.item()));
				return loss;
			closure.count = 0;
			optimizer.step(closure);
			with open(name, "wb") as fobj:
				pickle.dump(self.state_dict(), fobj);

	def display(self):
		w = None;
		for f in self.fc():
			if w is None:
				w = f.weight;
			else:
				w = f.weight.matmul(w);		
			display_network(w.data.numpy(), block = True);

class rica(nn.Module):
	def __init__(self, input_size, hidden_size):
		super(rica, self).__init__();
		self.weight = nn.Parameter(torch.Tensor(hidden_size, input_size), requires_grad = True);

	def initial(self):
		self.weight.data.normal_(0, 0.001);

	def forward(self, data):
		weight = self.weight / (self.weight ** 2).sum(dim = 1, keepdim = True).sqrt();
		active = data.matmul(weight.t());
		result = active.matmul(weight);
		self.rho = active.abs().sum() / data.shape[0];
		return result;

	def cost(self, data):
		result = self.__call__(data);
		loss = ((result - data) ** 2).sum() / data.shape[0] / 2;
		print(loss.data.item(), self.rho.data.item() * 0.01);
		loss += self.rho * 0.001;
		return loss;

	def train(self, data):
		optimizer = optim.LBFGS(self.parameters(), lr = 0.1, max_iter = 400);
		start = time.time();
		def closure():
			optimizer.zero_grad();
			loss = self.cost(data);
			loss.backward();
			closure.count += 1;
			duration = time.time() - start;
			print("count: %d, time: %f, loss: %f" % (closure.count, duration, loss.data.item()));
			return loss;
		closure.count = 0;
		optimizer.step(closure);

	def display(self):
		display_network(self.weight.data.numpy(), block = True);

class cnn(nn.Module):
	def __init__(self):
		super(cnn, self).__init__()
		self.conv1 = nn.Conv2d(1, 10, kernel_size = 5);
		self.conv2 = nn.Conv2d(10, 20, kernel_size = 5);
		self.fc1 = nn.Linear(320, 50);
		self.fc2 = nn.Linear(50, 10);

	def initial(self):
		self.conv1.weight.data.normal_(0, 0.001);
		self.conv1.bias.data.fill_(0);
		self.conv2.weight.data.normal_(0, 0.001);
		self.conv2.bias.data.fill_(0);
		self.fc1.weight.data.normal_(0, 0.001);
		self.fc1.bias.data.fill_(0);
		self.fc2.weight.data.normal_(0, 0.001);
		self.fc2.bias.data.fill_(0);

	def forward(self, data):
		data = fun.sigmoid(fun.avg_pool2d(self.conv1(data), 2));
		data = fun.sigmoid(fun.avg_pool2d(self.conv2(data), 2));
		data = data.view(-1, 320);
		data = fun.sigmoid(self.fc1(data));
		data = self.fc2(data);
		data -= data.max(dim = 1, keepdim = True)[0];
		data = fun.softmax(data, dim = 1);
		return data;

	def cost(self, data, label):
		result = self.__call__(data);
		loss = - result[np.array(range(label.shape[0])), label].log().mean();
		loss += 0.00001 * (self.fc2.weight ** 2).sum();
		return loss;

	def train(self, train_loader, epoch, lr, momentum):
		optimizer = optim.SGD(self.parameters(), lr = lr, momentum = momentum);
		for i in range(epoch):
			for index, (data, target) in enumerate(train_loader):
				data, label = Variable(data), Variable(target);
				optimizer.zero_grad();
				loss = self.cost(data, label);
				loss.backward();
				optimizer.step();
				if index % 10 == 0:
					print(
						"Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}".format(
							i + 1, index * len(data),
							len(train_loader.dataset),
							100. * index / len(train_loader),
							loss.data.item()
						)
					);

"""
	def train(self, data, label):
		optimizer = optim.LBFGS(self.parameters(), max_iter = 400, history_size = 25);
		start = time.time();
		def closure():
			optimizer.zero_grad();
			loss = self.cost(data, label);
			loss.backward();
			closure.count += 1;
			duration = time.time() - start;
			print("count: %d, time: %f, loss: %f" % (closure.count, duration, loss.data.item()));
			return loss;
		closure.count = 0;
		optimizer.step(closure);
		with open("cnn.dat", "wb") as fobj:
			pickle.dump(self.state_dict(), fobj);
"""
