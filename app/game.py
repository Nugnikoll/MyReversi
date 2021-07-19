# -*- coding: utf-8 -*-

import wx
import sys
import os
import time
import json
import shlex
import numpy as np
import shutil
import threading
import subprocess

if sys.platform == "win32":
	dll_suffix = ".pyd"
	exe_suffix = ".exe"
else:
	dll_suffix = ".so"
	exe_suffix = ""

if os.path.isdir("./lib") and not os.path.isfile("../_reversi" + dll_suffix):
	result = subprocess.run(["./cpuid" + exe_suffix], stdout = subprocess.PIPE)
	result = result.stdout.decode("utf-8")
	if not os.path.exists("./bot"):
		os.mkdir("./bot")
	if result.find("bmi2") >= 0 and result.find("avx2") >= 0:
		shutil.copy2("./lib/_reversi_avx2" + dll_suffix, "./_reversi" + dll_suffix)
		shutil.copy2("./lib/bot_avx2" + exe_suffix, "./bot/reversi" + exe_suffix)
	else:
		shutil.copy2("./lib/_reversi_asm" + dll_suffix, "./_reversi" + dll_suffix)
		shutil.copy2("./lib/bot_asm" + exe_suffix, "./bot/reversi" + exe_suffix)

sys.path.append("../python")
import reversi as rv

if os.path.isdir("./data/"):
	data_path = "./data/"
else:
	data_path = "../data/"

if os.path.isdir("./image/"):
	image_path = "./image/"
else:
	image_path = "../image/"

if os.path.isdir("./bot/"):
	bot_path = "./bot/"
else:
	bot_path = "../bot/"

if sys.platform == "win32":
	default_path = bot_path + "reversi.exe -c -t 900 -p " + data_path + "pattern.dat"
else:
	default_path = bot_path + "reversi -c -t 900 -p " + data_path + "pattern.dat"

bias = 34
num = 8
cell = 44
width = num * cell
cbias = bias + cell / 2
radius = cell / 2 - 4
thick = 3
margin = 20

mthd_default = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn

def check_pos(pos):
	return pos[0] >= 0 and pos[0] < rv.board.size and pos[1] >= 0 and pos[1] < rv.board.size

def format_pos(pos):
	tp = type(pos)
	if tp is tuple:
		if check_pos(pos):
			return pos[0] | pos[1] << 3
		else:
			return -1
	if tp is int:
		return pos
	if tp is str:
		if len(pos) != 2:
			return -1
		pos = [ord(pos[0]), ord(pos[1])]
		if pos[0] < pos[1]:
			pos = [pos[1], pos[0]]
		if pos[0] >= ord("a"):
			pos[0] -= ord("a")
		else:
			pos[0] -= ord("A")
		pos[1] -= ord("1")
		if check_pos(pos):
			return pos[0] | pos[1] << 3
		else:
			return -1
	return -1

def format_brd(brd):
	tp = type(brd)
	if tp is tuple:
		return rv.board(brd[0], brd[1])
	if tp is rv.board:
		return rv.board(brd)
	if tp is np.array:
		return rv.board(brd)

def choice2str(self):
	return "(\"%c%c\",%f)" % (chr((self.pos & 0x7) + ord("A")), chr((self.pos >> 3) + ord("1")), self.val)
setattr(rv.choice, "__str__", choice2str)

def vals2str(self):
	return "[" + ",".join([str(v) for v in self]) + "]"
setattr(rv.ints, "__str__", vals2str)
setattr(rv.floats, "__str__", vals2str)
setattr(rv.choices, "__str__", vals2str)

class player:
	def __init__(self, ply_type):
		self.type = ply_type
		self.path = default_path
		self.path_exec = ""
		self.proc = None

	def create_process(self):
		self.proc = subprocess.Popen(
			shlex.split(self.path_exec), stdin = subprocess.PIPE,
			stdout = subprocess.PIPE, stderr = subprocess.PIPE
		)

	def send(self, msg):
		if self.proc and self.path_exec != self.path:
			self.proc.kill()

		if self.proc is None or self.path_exec != self.path:
			self.path_exec = self.path
			self.create_process();
		else:
			poll = self.proc.poll()
			if not poll is None:
				self.create_process();

		if type(msg) is dict:
			msg = json.dumps(msg)
		if type(msg) is str:
			msg = msg.encode("utf-8")
		if len(msg) >= 0 and msg[-1] != b"\n":
			msg += b"\n"

		self.proc.stdin.write(msg)
		self.proc.stdin.flush()

	def recv(self, timeout = 4):
		poll = self.proc.poll()
		if not poll is None:
			errinfo = self.proc.stderr.read()
			if errinfo:
				raise RuntimeError("subprocess error: " + errinfo)

		proc = self.proc
		proc.flag_timeout = False
		def on_timeout():
			proc.flag_timeout = True
			proc.kill()
			proc.wait()
		tmr = threading.Timer(timeout, on_timeout)
		tmr.start()
		msg = self.proc.stdout.readline()
		tmr.cancel()
		if proc.flag_timeout:
			self.proc = None
			raise RuntimeError("subprocess timeout")

		if type(msg) is bytes:
			msg = msg.decode("utf-8")
		msg = json.loads(msg)
		if not (
			"control" in msg and "continue" in msg["control"]
			and msg["control"]["continue"]
		):
			self.proc.kill()
			self.proc.wait()
			self.proc = None
		return msg

class game:
	def __init__(self, app):
		self.app = app
		self.frame = app.frame
		self.panel_board = self.frame.panel_board
		self.text_log = self.frame.text_log

		self.brd = rv.board(0, 0)
		self.color = True
		self.pos = (-1, -1)
		self.mthd = mthd_default
		self.depth = -1
		self.flag_auto_save = True
		self.flag_lock = True
		self.ply = (
			player(rv.ply_ai), player(rv.ply_human)
		)
		self.record = []
		self.storage = []
		self.choices = None
		self.pv = None

	def show(self, dc = None):
		if dc is None:
			dc = wx.ClientDC(self.frame.panel_board)
		dc.Clear()
		
		#draw a board
		dc.DrawBitmap(self.frame.img_board, 0, 0)

		#draw valid moves
		brd_move = self.brd.get_move(self.color)
		for i in range(rv.board.size2):
			if brd_move & (1 << i):
				dc.DrawBitmap(self.frame.img_move, bias + cell * (i & 7), bias + cell * (i >> 3))

		#draw stones
		brd_white = self.brd.get_brd(False)
		brd_black = self.brd.get_brd(True)
		for i in range(rv.board.size2):
			if brd_white & (1 << i):
				dc.DrawBitmap(
					self.frame.img_stone[False][False],
					bias + cell * (i & 7),
					bias + cell * (i >> 3)
				)
			elif brd_black & (1 << i):
				dc.DrawBitmap(
					self.frame.img_stone[True][False],
					bias + cell * (i & 7),
					bias + cell * (i >> 3)
				)

		#show where is the last move
		if check_pos(self.pos):
			color = (self.get((self.pos[0], self.pos[1])) == rv.black)
			dc.DrawBitmap(
				self.frame.img_stone[color][True],
				bias + cell * self.pos[0],
				bias + cell * self.pos[1]
			)

		#show the evaluation
		if not (self.choices is None):
			dc.SetTextForeground(wx.Colour(255,30,30))
			dc.SetFont(
				wx.Font(
					9, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_NORMAL,
					wx.FONTWEIGHT_BOLD, False, "Consolas",
					wx.FONTENCODING_DEFAULT
				)
			)
			for c in self.choices:
				x = c.pos & 7
				y = c.pos >> 3
				s = "%.3f" % c.val
				dc.DrawText(
					s,
					bias + cell * x  + cell / 2 - 3.5 * len(s),
					bias + cell * y + cell / 2 - 8
				)

		#show the principle variation
		if not (self.pv is None):
			brd = rv.board(self.brd)
			color = self.color
			for i in range(len(self.pv)):
				x = self.pv[i] & 0x7
				y = self.pv[i] >> 3
				flag_move = (brd_move & (1 << self.pv[i]) != 0)
				dc.DrawBitmap(
					self.frame.img_pvs[flag_move][color][i],
					bias + cell * x,
					bias + cell * y
				)
				brd.flip(color, self.pv[i])
				status = brd.get_status(not color)
				if status & rv.sts_black:
					color = True
				else:
					color = False

	def mark(self, brd):
		dc = wx.ClientDC(self.frame.panel_board)
		dc.SetPen(wx.Pen(wx.Colour(0,0,230), thick))
		for i in range(rv.board.size2):
			if brd & (1 << i):
				x = i & 0x7
				y = i >> 3
				dc.DrawLine(bias + cell * x, bias + cell * y, bias + cell * (x + 1), bias + cell * (y + 1))
				dc.DrawLine(bias + cell * x, bias + cell * (y + 1), bias + cell * (x + 1), bias + cell * y)

	def paint(self, show_choice = False, show_pv = False):
		if not show_choice:
			self.choices = None
		if not show_pv:
			self.pv = None
		# wx.PostEvent(self.panel_board, wx.PaintEvent())
		evt = wx.PaintEvent(self.panel_board.GetId())
		self.panel_board.GetEventHandler().ProcessEvent(evt)

	def print_log(self, s):
		self.text_log.AppendText(s)

	def export(self, path, img_type = wx.BITMAP_TYPE_PNG):
		img = wx.Bitmap(424,424)
		memdc = wx.MemoryDC()
		memdc.SelectObject(img)
		memdc.SetBackground(wx.Brush(wx.Colour(43,155,0)))
		self.show(dc = memdc)
		img.SaveFile(path, img_type)

	def push(self):
		if self.flag_auto_save:
			self.record.append((rv.board(self.brd), self.color, self.pos))
			self.storage = []

	def pop(self):
		if len(self.record) == 0:
			return False
		else:
			self.storage.append((rv.board(self.brd), self.color, self.pos))
			(self.brd, self.color, self.pos) = self.record[-1]
			self.record.pop()
			return True

	def undo(self):
		result = self.pop()
		if result:
			self.print_log("undo\n")
			self.flag_lock = False
			self.paint()
		return result

	def redo(self):
		if len(self.storage) == 0:
			return False
		else:
			self.record.append((rv.board(self.brd), self.color, self.pos))
			(self.brd, self.color, self.pos) = self.storage[-1]
			self.storage.pop()
			self.print_log("redo\n")
			self.flag_lock = False
			self.paint()
			return True

	def set_color(self, color):
		self.push()
		self.color = color
		self.paint()

	def set_pos(self, pos):
		self.push()
		pos = format_pos(pos)
		self.pos = (pos & 0x7, pos >> 3)
		self.paint()

	def start(self):
		self.color = True
		self.pos = (-1,-1)
		self.record = []
		self.storage = []
		self.brd.initial()
		self.print_log("start a new game\n")
		self.flag_lock = False
		self.paint()
		self.play_continue()

	def get_brd(self, color = None):
		if color is None:
			return (self.brd.get_brd(False), self.brd.get_brd(True))
		else:
			return self.brd.get_brd(color)

	def assign(self, brd):
		self.push()
		self.brd = format_brd(brd)
		self.paint()
		self.print_log("assign new value to the board\n")

	def get(self, pos):
		return self.brd.get(format_pos(pos))

	def set(self, pos, chsman):
		self.push()
		self.brd.set(format_pos(pos), chsman)
		self.paint()

	def mirror_h(self):
		self.push()
		self.brd.mirror_h()
		self.pos = (7 - self.pos[0], self.pos[1])
		self.print_log("mirror horizontally\n")
		self.paint()

	def mirror_v(self):
		self.push()
		self.brd.mirror_v()
		self.pos = (self.pos[0], 7 - self.pos[1])
		self.print_log("mirror vertically\n")
		self.paint()

	def rotate_l(self):
		self.push()
		self.brd.rotate_l()
		self.pos = (self.pos[1], 7 - self.pos[0])
		self.print_log("rotate clockwise\n")
		self.paint()

	def rotate_r(self):
		self.push()
		self.brd.rotate_r()
		self.pos = (7 - self.pos[1], self.pos[0])
		self.print_log("rotate counterclockwise\n")
		self.paint()

	def reflect(self):
		self.push()
		self.brd.reflect()
		self.pos = (7 - self.pos[0], 7 - self.pos[1])
		self.print_log("reflect\n")
		self.paint()

	def reverse(self):
		self.push()
		self.brd.reverse()
		self.color = not self.color
		self.print_log("reverse\n")
		self.paint()

	def config(self):
		return self.brd.config()

	def flip(self, color, pos):

		pos = format_pos(pos)
		self.push()

		result = False

		# if the BMI2 instruction set is used by the function board::flip()
		# we do not need to compare self.brd.get(x + (y << 3)) with rv.blank
		if pos < 0 or self.brd.get(pos) != rv.blank:
			result = False
		else:
			brd_save = rv.board(self.brd)
			self.brd.flip(color, pos)
			result = (self.brd.get_brd(True) != brd_save.get_brd(True))

		if result:
			self.print_log(
				"place a " + ("black" if color else "white")
				+ " stone at " + chr((pos & 7) + ord("A")) + chr((pos >> 3) + ord("1")) + "\n"
			)
			if not (self.brd.get_status(not color) & rv.sts_again):
				self.color = not color
			self.pos = (pos & 7, pos >> 3)
			self.paint()
		else:
			if self.flag_auto_save:
				self.pop()
			self.print_log(
				"cannot place a " + ("black" if color else "white")
				+" stone here\n"
			)
		return result

	def count(self, color):
		return self.brd.count(color)

	def get_move(self, color):
		return self.brd.get_move(color)

	def count_move(self, color):
		return self.brd.count_move(color)

	def score(self, color):
		return self.brd.score(color)

	def process_method(self, mthd, depth):
		p_method = self.brd.process_method(mthd, depth)
		return p_method.first, p_method.second

	def search(self, mthd = None, color = None, depth = -1, alpha = rv._inf, beta = rv.inf):
		if mthd is None:
			mthd = self.mthd
		if color is None:
			color = self.color
		(mthd, depth) = self.process_method(mthd, depth)
		return self.brd.search(mthd, color, depth, alpha, beta)

	def get_choice(self, mthd = None, color = None, depth = -1):
		if mthd is None:
			mthd = self.mthd
		if color is None:
			color = self.color
		(mthd, depth) = self.process_method(mthd, depth)
		choices = self.brd.get_choice(mthd, color, depth)
		self.choices = rv.choices(choices)
		self.paint(show_choice = True)
		return choices

	def select_choice(self, choices):
		return self.brd.select_choice(choices)

	def get_pv(self, color = None):
		if color is None:
			color = self.color
		result = self.brd.get_pv(color)
		self.pv = result
		self.paint(show_pv = True)
		return result

	def play_ai(self, mthd = None, color = None, depth = None):

		if mthd is None:
			mthd = self.mthd
		if color is None:
			color = self.color
		if depth is None:
			depth = self.depth

		self.push()
		(mthd, depth) = self.process_method(mthd, depth)
		pos = self.brd.play(mthd, color, depth).pos
		if pos >= 0:
			pos = (pos & 7, pos >> 3)
		else:
			pos = (-1, -1)
		if pos[0] >= 0:
			self.print_log(
				"place a " + ("black" if color else "white")
				+ " stone at " + chr(pos[0] + ord("A")) + chr(pos[1] + ord("1")) + "\n"
			)
			if not (self.brd.get_status(not color) & rv.sts_again):
				self.color = not color
			self.pos = pos
			self.paint()
		else:
			self.print_log(
				("black" if color else "white")
				+ " is unable to move.\n"
			)
			if self.flag_auto_save:
				self.pop()

		return pos

	def play_other(self, mthd = None, color = None, depth = None):

		if mthd is None:
			mthd = self.mthd
		if color is None:
			color = self.color
		if depth is None:
			depth = self.depth

		ply = self.ply[color]

		request = {
			"request": {
				"color": color,
				"board": {
					"black": self.brd.get_brd(True),
					"white": self.brd.get_brd(False)
				}
			}
		}

		self.print_log(
			"send a request to process \""
			+ self.ply[color].path
			+ "\"\n"
		)

		print(request, file = self.frame.text_log)

		ply.send(request);

		self.print_log(
			"receive a response from process \""
			+ self.ply[color].path
			+ "\"\n"
		)

		response = ply.recv()

		print(response, file = self.frame.text_log)

		x = response["response"]["x"]
		y = response["response"]["y"]

		if not self.flip(color, (x, y)):
			raise RuntimeError("illegal move (%d,%d)" % (x, y))

	def play(self, mthd = None, color = None, depth = None):

		if mthd is None:
			mthd = self.mthd
		if color is None:
			color = self.color
		if depth is None:
			depth = self.depth

		p_type = self.ply[color].type
		if p_type == rv.ply_ai:
			self.play_ai(mthd, color, depth)
		elif p_type == rv.ply_other:
			self.play_other(mthd, color, depth)

	def play_continue(self, mthd = None, depth = None):

		if mthd is None:
			mthd = self.mthd
		if depth is None:
			depth = self.depth

		status = None

		while True:
			status = self.brd.get_status(self.color)
			if not (status & rv.sts_turn):
				break
			if self.ply[self.color].type == rv.ply_human:
				return
			wx.Yield()
			self.play(mthd, self.color, depth)

		self.flag_lock = True
		s = (
			str(self.brd.count(True)) + " black stones and "
			+ str(self.brd.count(False)) + " white stones remain\n"
		)
		if status == rv.sts_bwin:
			s += "black wins\n"
		elif status == rv.sts_wwin:
			s += "white wins\n"
		else:
			s += "a tie\n"
		self.print_log(s)

	def click(self, pos):
		status = self.brd.get_status(self.color)

		if not (status & rv.sts_turn):
			return

		if self.ply[self.color].type == rv.ply_human:
			self.flip(self.color, pos)
			wx.Yield()

		self.play(self.mthd, self.color, self.depth)
		self.play_continue()
