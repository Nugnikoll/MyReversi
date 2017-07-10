# -*- coding: utf-8 -*-

import wx
from wx import xrc
from reversi_gui import *
from wxs2xrc import *
import _thread
import time
import pdb

wxsfile = "../wxsmith/reversi_guiframe.wxs";
xrcfile = "../wxsmith/reversi_guiframe.xrc";
wxs2xrc(wxsfile,xrcfile);

rv.pattern.config();
rv.group.config("ptn_opt.dat")

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
		self.menubar = self.frame.GetMenuBar();
		self.menu_alg = self.menubar.FindItemById(xrc.XRCID("id_menu_alg")).GetSubMenu();
		self.menu_alg_rnd = self.menubar.FindItemById(xrc.XRCID("id_menu_alg_rnd"));
		self.menu_level = self.menubar.FindItemById(xrc.XRCID("id_menu_level")).GetSubMenu();
		self.statusbar = self.frame.GetStatusBar();

		self.id_menu_alg_rnd = xrc.XRCID("id_menu_alg_rnd");
		self.id_menu_alg_ab = xrc.XRCID("id_menu_alg_ab");
		self.id_menu_alg_kill = xrc.XRCID("id_menu_alg_kill");
		self.id_menu_alg_pvs = xrc.XRCID("id_menu_alg_pvs");
		self.id_menu_alg_trans = xrc.XRCID("id_menu_alg_trans");
		self.id_menu_alg_mtdf = xrc.XRCID("id_menu_alg_mtdf");
		self.id_menu_alg_ptn = xrc.XRCID("id_menu_alg_ptn");

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

		for item in self.menu_alg.GetMenuItems():
			self.Bind(wx.EVT_MENU,self.on_menu_alg,item);

		for item in self.menu_level.GetMenuItems():
			self.Bind(wx.EVT_MENU,self.on_menu_level,item);

		# Connect(id_book_tree,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,(wxObjectEventFunction)&reversi_guiFrame::on_tree_item_select);
		
		# self.panel_board.Connect(wxEVT_CONTEXT_MENU,wxContextMenuEventHandler(reversi_guiFrame::on_context_menu),NULL,this); 

		size_suit = self.frame.GetBestSize();
		self.frame.SetMinSize(size_suit);
		self.frame.SetSize(size_suit);
		self.frame.Show();
		self.thrd_lock = False;
		self.h_default = -1;

		mygame.dc = wx.ClientDC(self.panel_board);
		mygame.text_log = self.text_log;

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
		time_start = time.clock();
		exec(s);
		self.refresh();
		time_end = time.clock();
		self.statusbar.SetStatusText("Execution time : %f seconds" % (time_end - time_start),0);
	def on_text_input_textenter(self,event):
		self.process(self.text_input.GetValue());
	def on_black(self,event):
		self.process("mygame.start();");
	def on_white(self,event):
		self.process("mygame.start();");
		self.process("self._print(mygame.play(reversi.mthd_default,True));");
	def on_undo(self,event):
		self.process("mygame.undo();");
	def on_redo(self, event):
		self.process("mygame.redo();");
	def on_load(self, event):
		pass;
		# if self.dialog_load.ShowModal() == wxID_OK:
			# path = self.dialog_load.GetPath();
			# pos = 0;
			# path = path.replace("\\","\\\\");
			# self.process("load " + path);
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
			"self._print("
				"mygame.play("
					"reversi.coordinate(%d,%d),"
					"reversi.mthd_default,"
					"self.h_default"
				")"
			");"
			% (x,y)
		);
	def on_menu_alg(self,event):
		id = event.GetId();
		item = self.menu_alg.FindItemById(id);

		if id == self.id_menu_alg_rnd:
			if self.menu_alg_rnd.IsChecked():
				for ptr in self.menu_alg.GetMenuItems():
					ptr.Check(False);
				self.process("reversi.mthd_default = reversi.mthd_rnd")
			self.menu_alg_rnd.Check(True);
		else:
			if item.IsChecked():
				item.Check(True);
				self.menu_alg_rnd.Check(False);

				if id == self.id_menu_alg_ab:
					self.process("reversi.mthd_default |= reversi.mthd_ab");
				elif id == self.id_menu_alg_kill:
					self.process("reversi.mthd_default |= reversi.mthd_kill");
				elif id == self.id_menu_alg_pvs:
					self.process("reversi.mthd_default |= reversi.mthd_pvs");
				elif id == self.id_menu_alg_trans:
					self.process("reversi.mthd_default |= reversi.mthd_trans");
				elif id == self.id_menu_alg_mtdf:
					self.process("reversi.mthd_default |= reversi.mthd_mtdf");
				elif id == self.id_menu_alg_ptn:
					self.process("reversi.mthd_default |= reversi.mthd_ptn");
			else:
				flag = False;
				for ptr in self.menu_alg.GetMenuItems():
					if ptr.IsChecked():
						flag = True;
				if not flag:
					self.menu_alg_rnd.Check(True);

				if id == self.id_menu_alg_ab:
					self.process("reversi.mthd_default &= ~reversi.mthd_ab");
				elif id == self.id_menu_alg_kill:
					self.process("reversi.mthd_default &= ~reversi.mthd_kill");
				elif id == self.id_menu_alg_pvs:
					self.process("reversi.mthd_default &= ~reversi.mthd_pvs");
				elif id == self.id_menu_alg_trans:
					self.process("reversi.mthd_default &= ~reversi.mthd_trans");
				elif id == self.id_menu_alg_mtdf:
					self.process("reversi.mthd_default &= ~reversi.mthd_mtdf");
				elif id == self.id_menu_alg_ptn:
					self.process("reversi.mthd_default &= ~reversi.mthd_ptn");

	def on_menu_level(self,event):
		for item in self.menu_level.GetMenuItems():
			item.Check(False);

		pos = 0;
		(item,pos) = self.menu_level.FindChildItem(event.GetId());
		item.Check(True);
		if pos >= 7:
			pos = -1;

		self.process("self.h_default = %d" % pos);

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
		# mygame.color = True;
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