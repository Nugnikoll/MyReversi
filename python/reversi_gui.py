# -*- coding: utf-8 -*-

import wx
import reversi
import reversi as rv

bias = 34;
num = 8;
cell = 44;
width = num * cell;
cbias = bias + cell / 2;
radius = cell / 2 - 4;
thick = 3;
margin = 20;

grp = reversi.group();

def coord2str(self):
	return "(%d,%d)" % (self.x,self.y);
setattr(reversi.coordinate,"__str__",coord2str);

def choice2str(self):
	return "(%d,%d,%f)" % (self.pos & 7,self.pos >> 3,self.val);
setattr(reversi.choice,"__str__",choice2str);

def choices2str(self):
	return "[" + ",".join([choice2str(c) for c in self]) + "]";
setattr(reversi.choices,"__str__",choices2str);

def floats2str(self):
	return "[" + ",".join([str(f) for f in self]) + "]";
setattr(reversi.floats,"__str__",floats2str)

class game_gui(reversi.game):
	def show(self):
		self.do_show(self.dc);

	def do_show(self,dc):
		dc.Clear();

		#draw valid moves
		dc.SetBrush(wx.Brush(wx.Colour(30,100,0)));
		dc.SetPen(wx.Pen(wx.Colour(30,100,0),thick));
		brd_move = self.brd.get_move(self.color);
		for i in range(reversi.board.size2):
			if brd_move & (1 << i):
				dc.DrawRectangle(bias + cell * (i & 7),bias + cell * (i >> 3),cell,cell);
		
		#draw a board
		dc.SetPen(wx.Pen(wx.BLACK,thick));
		for i in range(num + 1):
			dc.DrawLine(bias,bias + cell * i,bias + width,bias + cell * i);
		for i in range(num + 1):
			dc.DrawLine(bias + cell * i,bias,bias + cell * i,bias + width);

		#draw the outline of the board
		dc.SetBrush(wx.BLACK_BRUSH);
		dc.SetPen(wx.Pen(wx.BLACK,thick));
		dc.DrawRectangle(bias - margin,bias - margin,margin,width + margin * 2);
		dc.DrawRectangle(bias - margin,bias - margin,width + margin * 2,margin);
		dc.DrawRectangle(bias + width,bias - margin,margin,width + margin * 2);
		dc.DrawRectangle(bias - margin,bias + width,width + margin * 2,margin);

		#draw coordinate labels
		dc.SetTextForeground(wx.Colour(190,190,190));
		dc.SetFont(
			wx.Font(
				12,wx.FONTFAMILY_SWISS,wx.FONTSTYLE_NORMAL,
				wx.FONTWEIGHT_BOLD,False,"Consolas"
				,wx.FONTENCODING_DEFAULT
			)
		);
		for i in range(num):
			dc.DrawText(chr(ord('A') + i),bias + cell / 2 + cell * i - 4,bias - margin / 2 - 10);
			dc.DrawText(chr(ord('A') + i),bias + cell / 2 + cell * i - 4,bias + width + margin / 2 - 12);
			dc.DrawText(chr(ord('1') + i),bias - margin / 2 - 4,bias + cell / 2 + cell * i - 10);
			dc.DrawText(chr(ord('1') + i),bias + width + margin / 2 - 5,bias + cell / 2 + cell * i - 10);

		#draw stones
		for i in range(num):
			for j in range(num):
				chssmn = self.brd.get(i + (j << 3));
				if chssmn == rv.black:
					dc.SetBrush(wx.Brush(wx.Colour(40,40,40)));
					dc.SetPen(wx.Pen(wx.Colour(20,20,20),thick));
					dc.DrawCircle(wx.Point(cbias + cell * i,cbias + cell * j),radius);
				elif chssmn == reversi.white:
					dc.SetBrush(wx.Brush(wx.Colour(210,210,210)));
					dc.SetPen(wx.Pen(wx.Colour(230,230,230),thick));
					dc.DrawCircle(wx.Point(cbias + cell * i,cbias + cell * j),radius);
		
		#show where is the last move
		if self.pos.check():
			if self.get(self.pos.x,self.pos.y) == rv.black:
				dc.SetBrush(wx.Brush(wx.Colour(50,50,30)));
				dc.SetPen(wx.Pen(wx.Colour(90,90,0),thick));
			else:
				dc.SetBrush(wx.Brush(wx.Colour(210,210,170)));
				dc.SetPen(wx.Pen(wx.Colour(200,200,30),thick));
			dc.DrawCircle(wx.Point(cbias + cell * self.pos.x,cbias + cell * self.pos.y),radius);

	def get_choice(self,mthd,color,depth):
		p_mthd = rv.game.process_method(self,mthd,depth);
		choices = self.brd.get_choice(p_mthd.first,color,p_mthd.second);

		self.dc.SetTextForeground(wx.Colour(255,30,30));
		self.dc.SetFont(
			wx.Font(
				9,wx.FONTFAMILY_SWISS,wx.FONTSTYLE_NORMAL,
				wx.FONTWEIGHT_BOLD,False,"Consolas",
				wx.FONTENCODING_DEFAULT
			)
		);
		for c in choices:
			x = c.pos & 7;
			y = c.pos >> 3;
			str = "%.3f" % c.val;
			self.dc.DrawText(
				str,
				bias + cell * x  + cell / 2 - 3.5 * len(str),
				bias + cell * y + cell / 2 - 8
			);

		return choices;

	def print_log(self,str):
		self.text_log.AppendText(str);

reversi.board.config();
mygame = game_gui();
mygame.flag_print_term = False;