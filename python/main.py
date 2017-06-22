# -*- coding: utf-8 -*-

import re
import codecs
import wx
from wx import xrc
import reversi
import _thread
import time
import pdb

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

wxsfile = "../wxsmith/reversi_guiframe.wxs"
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

xrcfile = "../wxsmith/reversi_guiframe.xrc"
fobj = codecs.open(xrcfile,"w","UTF-8");
fobj.writelines(lines_parse);
fobj.close();

evt_thrd_id = wx.NewId();
class thrd_event(wx.PyEvent):
	def __init__(self,data):
		wx.PyEvent.__init__(self);
		self.SetEventType(evt_thrd_id);
		self.data = data;

class reversi_app(wx.App):

	def OnInit(self):
		# pdb.set_trace();
		self.res = xrc.XmlResource(xrcfile);
		self.init_frame();
		return True;

	def init_frame(self):
		self.frame = self.res.LoadFrame(None, "reversi_guiFrame");
		self.panel_board = xrc.XRCCTRL(self.frame, "id_panel_board");
		self.text_input = xrc.XRCCTRL(self.frame, "id_text_input");
		self.text_term = xrc.XRCCTRL(self.frame, "id_text_term");
		self.text_log = xrc.XRCCTRL(self.frame, "id_text_log");

		self.panel_board.Bind(wx.EVT_PAINT,self.on_panel_board_paint);
		self.panel_board.Bind(wx.EVT_LEFT_DOWN,self.on_panel_board_leftdown);
		self.text_input.Bind(wx.EVT_TEXT_ENTER,self.on_text_input_textenter);
		self.Bind(wx.EVT_MENU,self.on_quit,id = xrc.XRCID("id_menu_quit"));
		self.Bind(wx.EVT_MENU,self.on_about,id = xrc.XRCID("id_menu_about"));
		self.Bind(wx.EVT_MENU,self.on_black,id = xrc.XRCID("id_menu_black"));
		self.Bind(wx.EVT_MENU,self.on_white,id = xrc.XRCID("id_menu_white"));
		self.Bind(wx.EVT_MENU,self.on_undo,id = xrc.XRCID("id_menu_undo"));
		self.Bind(wx.EVT_MENU,self.on_redo,id = xrc.XRCID("id_menu_redo"));
		self.Bind(wx.EVT_MENU,self.on_load,id = xrc.XRCID("id_menu_load"));
		self.Bind(wx.EVT_MENU,self.on_clear_log,id = xrc.XRCID("id_menu_clear_log"));
		self.Bind(wx.EVT_MENU,self.on_clear_term,id = xrc.XRCID("id_menu_clear_term"));
		self.Bind(wx.EVT_MENU,self.on_clear_all,id = xrc.XRCID("id_menu_clear"));
		self.Bind(wx.EVT_MENU,self.on_mirror_h,id = xrc.XRCID("id_menu_mirror_h"));
		self.Bind(wx.EVT_MENU,self.on_mirror_v,id = xrc.XRCID("id_menu_mirror_v"));
		self.Bind(wx.EVT_MENU,self.on_reflect,id = xrc.XRCID("id_menu_reflect"));
		self.Bind(wx.EVT_MENU,self.on_rotate_r,id = xrc.XRCID("id_menu_rotate_r"));
		self.Bind(wx.EVT_MENU,self.on_rotate_l,id = xrc.XRCID("id_menu_rotate_l"));
		self.Connect(-1,-1,evt_thrd_id,self.thrd_catch);

		# Connect(id_book_tree,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,(wxObjectEventFunction)&reversi_guiFrame::on_tree_item_select);
		
		# self.panel_board.Connect(wxEVT_CONTEXT_MENU,wxContextMenuEventHandler(reversi_guiFrame::on_context_menu),NULL,this); 

		size_suit = self.frame.GetBestSize();
		self.frame.SetMinSize(size_suit);
		self.frame.SetSize(size_suit);
		self.frame.Show();
		self.thrd_lock = False;

		mygame.dc = wx.ClientDC(self.panel_board);

	def on_quit(self,event):
		self.Close();
	def on_about(self,event):
		wx.MessageBox("Reversi Game\nBy Rick\n1300012743@pku.edu.cn","About");
	def on_panel_board_paint(self, event):
		self.paint();
	def paint(self):
		dc = wx.ClientDC(self.panel_board);
		mygame.do_show(dc)
	def _print(self,s):
		self.text_term.AppendText(str(s) + "\n");
	def refresh(self):
		if mygame.flag_log:
			self.text_log.AppendText(mygame.log_string);
			mygame.flag_log = False;
		if mygame.flag_show:
			self.paint();
			mygame.flag_show = False;
		if mygame.flag_term:
			self.text_term.AppendText(mygame.term_string);
			mygame.flag_term = False;
	def process(self,s):
		if self.thrd_lock:
			return;
		self.text_term.AppendText(">>" + s + "\n");
		exec(s);
		self.refresh();
	def on_text_input_textenter(self,event):
		self.process(self.text_input.GetValue());
	def on_black(self,event):
		self.process("mygame.start();");
	def on_white(self,event):
		self.process("mygame.start();");
		self.process(
			"result = mygame.play(reversi.mthd_default,True);\n"
			+ "result = (result.x,result.y);\n"
			+ "self._print(result);"
		);
	def on_undo(self,event):
		self.process("mygame.undo();");
	def on_redo(self, event):
		self.process("mygame.redo();");
	def on_load(self, event):
		if self.dialog_load.ShowModal() == wxID_OK:
			path = self.dialog_load.GetPath();
			pos = 0;
			path = path.replace("\\","\\\\");
			self.process("load " + path);
	def on_mirror_h(self, event):
		self.process("mygame.mirror_h();");
	def on_mirror_v(self, event):
		self.process("mygame.mirror_v();");
	def on_reflect(self, event):
		self.process("mygame.reflect();");
	def on_rotate_r(self, event):
		self.process("mygame.rotate_r();");
	def on_rotate_l(self, event):
		self.process("mygame.rotate_l();");
	def on_clear_log(self, event):
		self.process("self.text_log.Clear();");
	def on_clear_term(self, event):
		self.process("self.text_term.Clear();");
	def on_clear_all(self, event):
		self.process("self.text_log.Clear();");
		self.process("self.text_term.Clear();");
	def on_panel_board_leftdown(self,event):
		# if mygame.is_lock:
			# return;
		pos = event.GetPosition();
		x = int((pos.x - bias) / cell);
		y = int((pos.y - bias) / cell);
		self.process(
			"result = mygame.play(reversi.coordinate(" + str(x) + ","
			+ str(y) + "),reversi.mthd_default);\n"
			+ "result = (result.x,result.y);\n"
			+ "self._print(result);"
		)

	def thrd_launch(self,fun,param):
		self.thrd_lock = True;
		_thread.start_new_thread(fun,param);
	def thrd_catch(self,event):
		self.refresh();
		self.thrd_lock = False;
	def thrd_wrap(self,fun,param):
		result = fun(param);
		if result != None:
			mygame.term_string = str(result) + "\n";
			mygame.flag_term = True;
		wx.PostEvent(self,thrd_event(None));
	def sleep(self,count):
		time.sleep(count);
		mygame.term_string = "sleep for %d seconds\n" % count;
		mygame.flag_term = True;
		wx.PostEvent(self, thrd_event(None));

# def on_context_menu(wxContextMenuEvent& event){
	# //wxMenu* menu = new wxMenu();

	# //menu->Append(id_undo, _("&Undo"));  
# //	menu->Append(wxID_REDO, _("&Redo"));  
# //	menu->AppendSeparator();  
# //	menu->Append(wxID_CUT, _("Cu&t"));  
# //	menu->Append(wxID_COPY, _("&Copy"));  
# //	menu->Append(wxID_PASTE, _("&Paste"));  
# //	menu->Append(wxID_CLEAR, _("&Delete"));  
# //	menu->AppendSeparator();  
# //	menu->Append(wxID_SELECTALL, _("Select &All"));  
# //  
	# //PopupMenu(menu);
# }

# def on_tree_item_select(wxTreeEvent& event){
	# wxTreeItemId item = event.GetItem();
	# vector<node*> vec;
	# board brd;

	# while(item != book_tree->GetRootItem()){
		# vec.push_back(((myTreeItemData*)(book_tree->GetItemData(item)))->ptr);
		# item = book_tree->GetItemParent(item);
	# }
	# reverse(vec.begin(),vec.end());
	# if(!vec.empty()){
		# mygame.color = vec.back()->dat.color;
	# }else{
		# mygame.color = true;
	# }
	# brd.initial();
	# for(node *& ptr:vec){
		# brd.flip(!ptr->dat.color,ptr->dat.pos);
	# }
	# mygame.brd = brd;
	# mygame.show();
# }

if __name__ == "__main__":
	app = reversi_app(False);
	app.MainLoop();