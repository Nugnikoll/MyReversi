# -*- coding: utf-8 -*-

import wx;
import sys;
import os;
import reversi;
import reversi as rv;
import json;
import subprocess as sp;
from enum import Enum;

bias = 34;
num = 8;
cell = 44;
width = num * cell;
cbias = bias + cell / 2;
radius = cell / 2 - 4;
thick = 3;
margin = 20;

def check(pos):
	return pos[0] >= 0 and pos[0] < rv.board.size and pos[1] >= 0 and pos[1] < rv.board.size;

def coord2str(self):
	return "(%d,%d)" % (self.x,self.y);
setattr(reversi.coordinate, "__str__", coord2str);

def choice2str(self):
	return "(%d,%d,%f)" % (self.pos & 7,self.pos >> 3,self.val);
setattr(reversi.choice, "__str__", choice2str);

def vals2str(self):
	return "[" + ",".join([str(v) for v in self]) + "]";
setattr(reversi.ints, "__str__", vals2str);
setattr(reversi.floats, "__str__", vals2str);
setattr(reversi.choices, "__str__", vals2str);

if sys.platform == "win32":
	default_path = "../bot/reversi.exe";
else:
	default_path = "../bot/reversi";

class player:
	pass;

class game:
	def __init__(self):
		self.brd = rv.board(0, 0);
		self.color = True;
		self.pos = (-1, -1);
		self.mthd = rv.mthd_ab | rv.mthd_kill | rv.mthd_pvs | rv.mthd_trans | rv.mthd_mtdf | rv.mthd_ptn;
		self.depth = -1;
		self.flag_auto_save = True;
		self.flag_lock = True;
		self.ply = (player(), player());
		self.record = [];
		self.storage = [];
		self.choices = None;
		self.pv = None;

		self.ply[False].type = rv.ply_ai;
		self.ply[False].path = default_path
		self.ply[True].type = rv.ply_human;
		self.ply[True].path = default_path

	def show(self):
		self.do_show(self.dc);

	def do_show(self, dc):
		dc.Clear();

		#draw valid moves
		dc.SetBrush(wx.Brush(wx.Colour(30,100,0)));
		dc.SetPen(wx.Pen(wx.Colour(30,100,0), thick));
		brd_move = self.brd.get_move(self.color);
		for i in range(reversi.board.size2):
			if brd_move & (1 << i):
				dc.DrawRectangle(bias + cell * (i & 7), bias + cell * (i >> 3), cell, cell);
		
		#draw a board
		dc.SetPen(wx.Pen(wx.BLACK, thick));
		for i in range(num + 1):
			dc.DrawLine(bias, bias + cell * i, bias + width, bias + cell * i);
		for i in range(num + 1):
			dc.DrawLine(bias + cell * i, bias, bias + cell * i, bias + width);

		#draw the outline of the board
		dc.SetBrush(wx.BLACK_BRUSH);
		dc.SetPen(wx.Pen(wx.BLACK,thick));
		dc.DrawRectangle(bias - margin, bias - margin, margin, width + margin * 2);
		dc.DrawRectangle(bias - margin, bias - margin, width + margin * 2, margin);
		dc.DrawRectangle(bias + width, bias - margin, margin, width + margin * 2);
		dc.DrawRectangle(bias - margin, bias + width, width + margin * 2, margin);

		#draw coordinate labels
		dc.SetTextForeground(wx.Colour(190,190,190));
		dc.SetFont(
			wx.Font(
				12, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_NORMAL,
				wx.FONTWEIGHT_BOLD,False, "Consolas"
				,wx.FONTENCODING_DEFAULT
			)
		);
		for i in range(num):
			dc.DrawText(chr(ord('A') + i), bias + cell / 2 + cell * i - 4, bias - margin / 2 - 10);
			dc.DrawText(chr(ord('A') + i), bias + cell / 2 + cell * i - 4, bias + width + margin / 2 - 12);
			dc.DrawText(chr(ord('1') + i), bias - margin / 2 - 4,bias + cell / 2 + cell * i - 10);
			dc.DrawText(chr(ord('1') + i), bias + width + margin / 2 - 5, bias + cell / 2 + cell * i - 10);

		#draw stones
		for i in range(num):
			for j in range(num):
				chssmn = self.brd.get(i + (j << 3));
				if chssmn == rv.black:
					dc.SetBrush(wx.Brush(wx.Colour(40,40,40)));
					dc.SetPen(wx.Pen(wx.Colour(20,20,20), thick));
					dc.DrawCircle(wx.Point(cbias + cell * i, cbias + cell * j),radius);

				elif chssmn == reversi.white:
					dc.SetBrush(wx.Brush(wx.Colour(210,210,210)));
					dc.SetPen(wx.Pen(wx.Colour(230,230,230), thick));
					dc.DrawCircle(wx.Point(cbias + cell * i, cbias + cell * j),radius);
		
		#show where is the last move
		if check(self.pos):
			if self.get(self.pos[0], self.pos[1]) == rv.black:
				dc.SetBrush(wx.Brush(wx.Colour(50,50,30)));
				dc.SetPen(wx.Pen(wx.Colour(90,90,0), thick));
			else:
				dc.SetBrush(wx.Brush(wx.Colour(210,210,170)));
				dc.SetPen(wx.Pen(wx.Colour(200,200,30), thick));
			dc.DrawCircle(wx.Point(cbias + cell * self.pos[0], cbias + cell * self.pos[1]),radius);

		if not (self.choices is None):
			dc.SetTextForeground(wx.Colour(255,30,30));
			dc.SetFont(
				wx.Font(
					9, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_NORMAL,
					wx.FONTWEIGHT_BOLD, False, "Consolas",
					wx.FONTENCODING_DEFAULT
				)
			);
			for c in self.choices:
				x = c.pos & 7;
				y = c.pos >> 3;
				s = "%.3f" % c.val;
				dc.DrawText(
					s,
					bias + cell * x  + cell / 2 - 3.5 * len(s),
					bias + cell * y + cell / 2 - 8
				);

		if not (self.pv is None):
			brd = rv.board(self.brd);
			color = self.color;
			for i in range(len(self.pv)):
				x = self.pv[i] & 0x7;
				y = self.pv[i] >> 3;
				if color:
					dc.SetBrush(wx.Brush(wx.TransparentColour));
					dc.SetPen(wx.Pen(wx.Colour(20,20,20), thick));
					dc.DrawCircle(wx.Point(cbias + cell * x, cbias + cell * y), radius);
					dc.SetTextForeground(wx.Colour(20,20,20));
					s = "%d" % (i + 1);
					dc.DrawText(
						s,
						bias + cell * x  + cell / 2 - 3.5 * len(s),
						bias + cell * y + cell / 2 - 8
					);
				else:
					dc.SetBrush(wx.Brush(wx.TransparentColour));
					dc.SetPen(wx.Pen(wx.Colour(230,230,230), thick));
					dc.DrawCircle(wx.Point(cbias + cell * x, cbias + cell * y), radius);
					dc.SetTextForeground(wx.Colour(230,230,230));
					s = "%d" % (i + 1);
					dc.DrawText(
						s,
						bias + cell * x  + cell / 2 - 3.5 * len(s),
						bias + cell * y + cell / 2 - 8
					);
				brd.flip(color, self.pv[i]);
				status = brd.get_status(not color);
				if status & rv.sts_black:
					color = True;
				else:
					color = False;

	def paint(self, show_choice = False, show_pv = False):
		if not show_choice:
			self.choices = None;
		if not show_pv:
			self.pv = None;
		evt = wx.PaintEvent(self.panel_board.GetId());
		self.panel_board.GetEventHandler().ProcessEvent(evt);

	def print_log(self, s):
		self.text_log.AppendText(s);

	def export(self, path, img_type = wx.BITMAP_TYPE_PNG):
		img = wx.Bitmap(424,424);
		memdc = wx.MemoryDC();
		memdc.SelectObject(img);
		memdc.SetBackground(wx.Brush(wx.Colour(43,155,0)));
		self.do_show(memdc);
		img.SaveFile(path, img_type);

	def push(self):
		if self.flag_auto_save:
			self.record.append((rv.board(self.brd), self.color, self.pos));
			self.storage = [];

	def pop(self):
		if len(self.record) == 0:
			return False;
		else:
			self.storage.append((rv.board(self.brd), self.color, self.pos));
			(self.brd, self.color, self.pos) = self.record[-1];
			self.record.pop();
			return True;

	def undo(self):
		result = self.pop();
		if result:
			self.print_log("undo\n");
			self.flag_lock = False;
			self.paint();
		return result;

	def redo(self):
		if len(self.storage) == 0:
			return False;
		else:
			self.record.append((rv.board(self.brd), self.color, self.pos));
			(self.brd, self.color, self.pos) = self.storage[-1];
			self.storage.pop();
			self.print_log("redo\n");
			self.flag_lock = False;
			self.paint();
			return True;

	def set_color(self, color):
		self.push();
		self.color = color;
		self.paint();

	def set_pos(self, x, y):
		self.push();
		self.pos = (x, y);
		self.paint();

	def start(self):
		self.color = True;
		self.pos = (-1,-1);
		self.record = [];
		self.storage = [];
		self.brd.initial();
		self.print_log("start a new game\n");
		self.flag_lock = False;
		self.paint();
		self.play_continue();

	def bget(self, color):
		return self.brd.bget(color);

	def assign(self, brd):
		self.push();
		self.brd = brd;
		self.paint();
		self.print_log("assign new value to the board\n");

	def get(self, x, y):
		return self.brd.get(x + (y << 3));

	def set(self, x, y, chsman):
		self.push();
		self.brd.set(x + (y << 3), chsman);
		self.paint();

	def mirror_h(self):
		self.push();
		self.brd.mirror_h();
		self.pos = (7 - self.pos[0], self.pos[1]);
		self.print_log("mirror horizontally\n");
		self.paint();

	def mirror_v(self):
		self.push();
		self.brd.mirror_v();
		self.pos = (self.pos[0], 7 - self.pos[1]);
		self.print_log("mirror vertically\n");
		self.paint();

	def rotate_l(self):
		self.push();
		self.brd.rotate_l();
		self.pos = (self.pos[1], 7 - self.pos[0]);
		self.print_log("rotate clockwise\n");
		self.paint();

	def rotate_r(self):
		self.push();
		self.brd.rotate_r();
		self.pos = (7 - self.pos[1], self.pos[0]);
		self.print_log("rotate counterclockwise\n");
		self.paint();

	def reflect(self):
		self.push();
		self.brd.reflect();
		self.pos = (7 - self.pos[0], 7 - self.pos[1]);
		self.print_log("reflect\n");
		self.paint();

	def reverse(self):
		self.push();
		self.brd.reverse();
		self.color = not color;
		self.print_log("reverse\n");
		self.paint();

	def config(self):
		return brd.config();

	def flip(self, color, x, y):

		self.push();

		result = False;

		# if the BMI2 instruction set is used by the function board::flip()
		# we do not need to compare self.brd.get(x + (y << 3)) with rv.blank
		if not check((x,y)) or self.brd.get(x + (y << 3)) != rv.blank:
			result = False;
		else:
			brd_save = rv.board(self.brd);
			self.brd.flip(color, x + (y << 3));
			result = (self.brd.bget(True) != brd_save.bget(True));

		if result:
			self.print_log(
				"place a " + ("black" if color else "white")
				+ " stone at " + chr(x + ord("A")) + chr(y + ord("1")) + "\n"
			);
			if not (self.brd.get_status(not color) & rv.sts_again):
				self.color = not color;
			self.pos = (x,y);
			self.paint();
		else:
			if self.flag_auto_save:
				self.pop();
			self.print_log(
				"cannot place a " + ("black" if color else "white")
				+" stone here\n"
			);
		return result;

	def count(self, color):
		return self.brd.count(color);

	def get_move(self, color):
		return self.brd.get_move(color);

	def count_move(self, color):
		return self.brd.count_move(color);

	def score(self, color):
		return self.brd.score(color);

	def process_method(self, mthd, depth):
		result = [mthd, depth];
		total = self.brd.sum();

		if result[0] == rv.mthd_rnd:
			return result;

		if result[1] == -1:
			if total <= 7:
				result[1] = 10;
			elif total <= 10:
				result[1] = 9;
			elif total <= rv.board.size2 - 22:
				result[1] = 8;
			elif total <= rv.board.size2 - 15:
				result[1] = 9;
			else:
				result[1] = 20;
		elif result[1] == -2:
			if total <= 7:
				result[1] = 10;
			elif total <= 10:
				result[1] = 10;
			elif total <= rv.board.size2 - 24:
				result[1] = 9;
			elif total <= rv.board.size2 - 16:
				result[1] = 10;
			else:
				result[1] = 20;
		elif result[1] <= -3:
			if total <= 7:
				result[1] = 11;
			elif total <= 10:
				result[1] = 11;
			elif total <= rv.board.size2 - 24:
				result[1] = 10;
			elif total <= rv.board.size2 - 16:
				result[1] = 11;
			else:
				result[1] = 20;
		if result[1] >= rv.board.size2 - total:
			result[0] |= rv.mthd_end;
			result[1] = rv.board.size2 - total;

		return result;

	def search(self, mthd, color, depth = -1, alpha = rv._inf, beta = rv.inf):
		(mthd, depth) = self.process_method(mthd, depth);
		return self.brd.search(mthd, color, depth, alpha, beta);

	def get_choice(self, mthd, color, depth = -1):
		(mthd, depth) = self.process_method(mthd, depth);
		choices = self.brd.get_choice(mthd, color, depth);
		self.choices = rv.choices(choices);
		self.paint(show_choice = True);
		return choices;

	def select_choice(self, choices):
		return self.brd.select_choice(choices);

	def get_pv(self, color):
		result = self.brd.get_pv(color);
		self.pv = result;
		self.paint(show_pv = True);
		return result;

	def play_ai(self, mthd = None, color = None, depth = None):

		if mthd is None:
			mthd = self.mthd;
		if color is None:
			color = self.color;
		if depth is None:
			depth = self.depth;

		self.push();
		(mthd, depth) = self.process_method(mthd, depth);
		pos = self.brd.play(mthd, color, depth);
		pos = (pos.x, pos.y);
		if pos[0] >= 0:
			self.print_log(
				"place a " + ("black" if color else "white")
				+ " stone at " + chr(pos[0] + ord("A")) + chr(pos[1] + ord("1")) + "\n"
			);
			if not (self.brd.get_status(not color) & rv.sts_again):
				self.color = not color;
			self.pos = pos;
			self.paint();
		else:
			self.print_log(
				("black" if color else "white")
				+ " is unable to move.\n"
			);
			if self.flag_auto_save:
				self.pop();

		return pos;

	def play_other(self, mthd = None, color = None, depth = None):

		if mthd is None:
			mthd = self.mthd;
		if color is None:
			color = self.color;
		if depth is None:
			depth = self.depth;

		result = rv.coordinate();
		request = {
			"request":{
				"color":color,
				"board":{
					"black":self.brd.bget(True),
					"white":self.brd.bget(False)
				}
			}
		};

		s = json.dumps(request);

		self.text_log.AppendText(
			"send a request to process \""
			+ self.ply[color].path
			+ "\"\n"
		);
		self.text_log.AppendText(s + "\n");

		sp_result = sp.run(self.ply[color].path.strip().split(), input = s.encode(), stdout = sp.PIPE);

		if sp_result.returncode:
			self.text_log.AppendText(
				"runtime error\n"
				 + "the process exits with code %d" % sp_result.returncode
			);
			raise RuntimeError;

		self.text_log.AppendText(
			"receive a response from process \""
			+ self.ply[color].path
			+ "\"\n"
		);

		response = None;
		self.text_log.AppendText(sp_result.stdout.decode());
		try:
			response = json.loads(sp_result.stdout.decode());
		except json.decoder.JSONDecodeError:
			self.text_log.AppendText(
				"invalid output format\n"
			);
			raise RuntimeError;

		if not "response" in response or not "x" in response["response"] or not "y" in response["response"]:
			self.text_log.AppendText(
				"invalid output format\n"
			);
			raise RuntimeError;

		result.x = response["response"]["x"];
		result.y = response["response"]["y"];

		if(not self.flip(color, result.x, result.y)):
			self.text_log.AppendText(
				"illegal move\n"
			);
			raise RuntimeError;

	def play(self, mthd = None, color = None, depth = None):

		if mthd is None:
			mthd = self.mthd;
		if color is None:
			color = self.color;
		if depth is None:
			depth = self.depth;

		p_type = self.ply[color].type;
		if p_type == rv.ply_ai:
			self.play_ai(mthd, color, depth);
		elif p_type == rv.ply_other:
			self.play_other(mthd, color, depth);

	def play_continue(self, mthd = None, depth = None):

		if mthd is None:
			mthd = self.mthd;
		if depth is None:
			depth = self.depth;

		status = None;

		while True:
			status = self.brd.get_status(self.color);
			if not (status & rv.sts_turn):
				break;
			if self.ply[self.color].type == rv.ply_human:
				return;
			wx.Yield();
			self.play(mthd, self.color, depth);

		self.flag_lock = True;
		s = (
			str(self.brd.count(True)) + " black stones and "
			+ str(self.brd.count(False)) + " white stones remain\n"
		);
		if status == rv.sts_bwin:
			s += "black wins\n";
		elif status == rv.sts_wwin:
			s += "white wins\n";
		else:
			s += "a tie\n";
		self.print_log(s);

	def click(self, pos):
		status = self.brd.get_status(self.color);

		if not (status & rv.sts_turn):
			return;

		if self.ply[self.color].type == rv.ply_human:
			self.flip(self.color, pos[0], pos[1]);

		try:
			self.play(self.mthd, self.color, self.depth);
			self.play_continue();
		except RuntimeError:
			pass;

reversi.board.config();
reversi.pattern.config();
mygame = game();
