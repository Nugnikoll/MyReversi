# -*- coding: utf-8 -*-

import wx
import reversi

def coord2str(self):
	return "(%d,%d)" % (self.x,self.y);
setattr(reversi.coordinate,"__str__",coord2str);

def choice2str(self):
	return "(%d,%d,%f)" % (self.pos >> 3,self.pos & 7,self.val);
setattr(reversi.choice,"__str__",choice2str);

def choices2str(self):
	return "[" + ",".join([choice2str(c) for c in self]) + "]";
setattr(reversi.choices,"__str__",choices2str);

bias = 10;
num = 8;
cell = 50;
width = num * cell;
cbias = bias + cell / 2;
radius = cell / 2 - 5;

class game_gui(reversi.game):
	def show(self):
		self.do_show(self.dc);
	def do_show(self,dc):
		dc.Clear();
		
		#draw valid moves
		dc.SetBrush(wx.Brush(wx.Colour(23,95,0)));
		dc.SetPen(wx.Pen(wx.Colour(23,95,0),4));
		brd_move = self.brd.get_move(self.color);
		for i in range(reversi.board.size2):
			if brd_move & (1 << i):
				dc.DrawRectangle(bias + cell * (i & 7),bias + cell * (i >> 3),cell,cell);
		
		#draw a board
		dc.SetPen(wx.Pen(wx.BLACK,4));
		for i in range(num + 1):
			dc.DrawLine(bias,bias + cell * i,bias + width,bias + cell * i);
		for i in range(num + 1):
			dc.DrawLine(bias + cell * i,bias,bias + cell * i,bias + width);
		
		for i in range(num):
			for j in range(num):
				chssmn = self.brd.get(i + (j << 3));
				if chssmn == reversi.black:
					dc.SetBrush(wx.Brush(wx.Colour(40,40,40)));
					dc.SetPen(wx.Pen(wx.Colour(20,20,20),4));
					dc.DrawCircle(wx.Point(cbias + cell * i,cbias + cell * j),radius);
				elif chssmn == reversi.white:
					dc.SetBrush(wx.Brush(wx.Colour(210,210,210)));
					dc.SetPen(wx.Pen(wx.Colour(230,230,230),4));
					dc.DrawCircle(wx.Point(cbias + cell * i,cbias + cell * j),radius);
		
		#show where is the last move
		dc.SetBrush(wx.TRANSPARENT_BRUSH);
		dc.SetPen(wx.Pen(wx.YELLOW,4));
		dc.DrawCircle(wx.Point(cbias + cell * self.pos.x,cbias + cell * self.pos.y),radius);

reversi.board.config();
mygame = game_gui();
mygame.flag_print_term = False;