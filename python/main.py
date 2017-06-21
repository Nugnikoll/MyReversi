# -*- coding: utf-8 -*-

import wx
import reversi as rv

rv.board.config();
mygame = rv.game();

reversi_app = wx.App();

id_panel_board = wx.NewId();
id_text_label = wx.NewId();
id_text_terminal = wx.NewId();
ID_NOTEBOOK1 = wx.NewId();
id_text_input = wx.NewId();
id_panel_base = wx.NewId();
id_player_black = wx.NewId();
id_player_white = wx.NewId();
id_new_game = wx.NewId();
id_load = wx.NewId();
id_save = wx.NewId();
id_quit = wx.NewId();
id_undo = wx.NewId();
id_redo = wx.NewId();
id_horizontal = wx.NewId();
id_vertical = wx.NewId();
id_reflect = wx.NewId();
id_clockwise = wx.NewId();
id_counterclockwise = wx.NewId();
ID_MENUITEM7 = wx.NewId();
id_clear_log = wx.NewId();
id_clear_term = wx.NewId();
id_clear_all = wx.NewId();
ID_MENUITEM1 = wx.NewId();
ID_MENUITEM2 = wx.NewId();
ID_MENUITEM3 = wx.NewId();
ID_MENUITEM4 = wx.NewId();
ID_MENUITEM5 = wx.NewId();
ID_MENUITEM6 = wx.NewId();
id_algorithm = wx.NewId();
id_level = wx.NewId();
id_about = wx.NewId();
ID_STATUSBAR1 = wx.NewId();
id_text_log = wx.NewId();
id_book_tree = wx.NewId();

class reversi_frame(wx.Frame):
	def __init__(self, parent):
		super(reversi_frame, self).__init__(parent, wx.ID_ANY, "Reversi", wx.DefaultPosition, wx.DefaultSize);
		frame_icon = wx.Icon();
		frame_icon.CopyFromBitmap(wx.Bitmap(wx.Image(r"E:\LLK\ComputerSci\Cplusplus\reversi\icon\Reversi.bmp")));
		self.SetIcon(frame_icon);
		self.Show();
		box_sizer_base = wx.BoxSizer(wx.HORIZONTAL);
		panel_base = wx.Panel(self, id_panel_base, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL, "id_panel_base");
		panel_base.SetBackgroundColour(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BACKGROUND));
		box_sizer_2 = wx.BoxSizer(wx.HORIZONTAL);
		box_sizer_3 = wx.BoxSizer(wx.VERTICAL);
		self.panel_board = wx.Panel(panel_base, id_panel_board, wx.DefaultPosition, wx.Size(424,424), wx.SUNKEN_BORDER|wx.TAB_TRAVERSAL, "id_panel_board");
		self.panel_board.SetBackgroundColour(wx.Colour(43,155,0));
		box_sizer_3.Add(self.panel_board, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 5);
		box_sizer_2.Add(box_sizer_3, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 5);
		box_sizer_4 = wx.BoxSizer(wx.VERTICAL);
		self.text_label = wx.StaticText(panel_base, id_text_label, "Reversi", wx.DefaultPosition, wx.Size(154,51), 0, "id_text_label");
		self.text_label.SetForegroundColour(wx.Colour(200,200,200));
		text_labelFont = wx.Font(26,wx.FONTFAMILY_SWISS,wx.FONTSTYLE_NORMAL,wx.FONTWEIGHT_BOLD,False, "Segoe Script",wx.FONTENCODING_DEFAULT);
		self.text_label.SetFont(text_labelFont);
		box_sizer_4.Add(self.text_label, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 5);
		Notebook1 = wx.Notebook(panel_base, ID_NOTEBOOK1, wx.DefaultPosition, wx.Size(448,323), 0, "ID_NOTEBOOK1");
		Notebook1Font = wx.Font(12,wx.FONTFAMILY_SWISS,wx.FONTSTYLE_NORMAL,wx.FONTWEIGHT_BOLD,False, "Consolas",wx.FONTENCODING_DEFAULT);
		Notebook1.SetFont(Notebook1Font);
		self.text_terminal = wx.TextCtrl(Notebook1, id_text_terminal, wx.EmptyString, wx.Point(-81,23), wx.Size(283,296), wx.TE_MULTILINE|wx.TE_READONLY, wx.DefaultValidator, "id_text_terminal");
		self.text_terminal.SetForegroundColour(wx.Colour(200,200,200));
		self.text_terminal.SetBackgroundColour(wx.Colour(32,32,32));
		self.text_terminalFont = wx.Font(14,wx.FONTFAMILY_SWISS,wx.FONTSTYLE_NORMAL,wx.FONTWEIGHT_BOLD,False,"Consolas",wx.FONTENCODING_DEFAULT);
		self.text_terminal.SetFont(self.text_terminalFont);
		Notebook1.AddPage(self.text_terminal, wx.EmptyString, False);
		box_sizer_4.Add(Notebook1, 1, wx.ALL|wx.EXPAND, 5);
		self.text_input = wx.TextCtrl(panel_base, id_text_input, wx.EmptyString, wx.DefaultPosition, wx.Size(266,30), wx.TE_PROCESS_ENTER|wx.TE_PROCESS_TAB, wx.DefaultValidator, "id_text_input");
		self.text_input.SetForegroundColour(wx.Colour(200,200,200));
		self.text_input.SetBackgroundColour(wx.Colour(32,32,32));
		self.text_inputFont = wx.Font(14,wx.FONTFAMILY_SWISS,wx.FONTSTYLE_NORMAL,wx.FONTWEIGHT_BOLD,False,"Consolas",wx.FONTENCODING_DEFAULT);
		self.text_input.SetFont(self.text_inputFont);
		box_sizer_4.Add(self.text_input, 0, wx.ALL|wx.EXPAND, 5);
		box_sizer_2.Add(box_sizer_4, 1, wx.ALL|wx.EXPAND, 5);
		panel_base.SetSizer(box_sizer_2);
		box_sizer_2.Fit(panel_base);
		box_sizer_2.SetSizeHints(panel_base);
		box_sizer_base.Add(panel_base, 1, wx.ALL|wx.EXPAND, 5);
		self.SetSizer(box_sizer_base);
		MenuBar1 = wx.MenuBar();
		Menu1 = wx.Menu();
		MenuItem3 = wx.Menu();
		MenuItem4 = wx.MenuItem(MenuItem3, id_player_black, "Player &Black\tCtrl-B", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem3.Append(MenuItem4);
		MenuItem5 = wx.MenuItem(MenuItem3, id_player_white, "Player &White\tCtrl-W", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem3.Append(MenuItem5);
		Menu1.Append(id_new_game, "New Game", MenuItem3, wx.EmptyString);
		MenuItem7 = wx.MenuItem(Menu1, id_load, "&Load\tCtrl-L", wx.EmptyString, wx.ITEM_NORMAL);
		Menu1.Append(MenuItem7);
		MenuItem8 = wx.MenuItem(Menu1, id_save, "&Save\tCtrl-S", wx.EmptyString, wx.ITEM_NORMAL);
		Menu1.Append(MenuItem8);
		MenuItem1 = wx.MenuItem(Menu1, id_quit, "&Quit\tAlt-F4", "Quit the application", wx.ITEM_NORMAL);
		Menu1.Append(MenuItem1);
		MenuBar1.Append(Menu1, "&File");
		Menu4 = wx.Menu();
		MenuItem10 = wx.MenuItem(Menu4, id_undo, "&Undo\tCtrl-Z", wx.EmptyString, wx.ITEM_NORMAL);
		Menu4.Append(MenuItem10);
		MenuItem11 = wx.MenuItem(Menu4, id_redo, "&Redo\tCtrl-Y", wx.EmptyString, wx.ITEM_NORMAL);
		Menu4.Append(MenuItem11);
		MenuItem18 = wx.Menu();
		MenuItem22 = wx.MenuItem(MenuItem18, id_horizontal, "Mirror &Horizontally\tAlt+H", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem18.Append(MenuItem22);
		MenuItem23 = wx.MenuItem(MenuItem18, id_vertical, "Mirror &Vertically\tAlt+V", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem18.Append(MenuItem23);
		MenuItem21 = wx.MenuItem(MenuItem18, id_reflect, "&Reflect\tAlt+R", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem18.Append(MenuItem21);
		MenuItem19 = wx.MenuItem(MenuItem18, id_clockwise, "Rotate &Clockwise\tAlt+C", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem18.Append(MenuItem19);
		MenuItem20 = wx.MenuItem(MenuItem18, id_counterclockwise, "Rotate Coun&terclockwise\tAlt+T", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem18.Append(MenuItem20);
		Menu4.Append(ID_MENUITEM7, "&Transform", MenuItem18, wx.EmptyString);
		MenuItem25 = wx.Menu();
		MenuItem24 = wx.MenuItem(MenuItem25, id_clear_log, "&Log", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem25.Append(MenuItem24);
		MenuItem26 = wx.MenuItem(MenuItem25, id_clear_term, "&Terminal", wx.EmptyString, wx.ITEM_NORMAL);
		MenuItem25.Append(MenuItem26);
		Menu4.Append(id_clear_all, "&Clear", MenuItem25, wx.EmptyString);
		MenuBar1.Append(Menu4, "&Edit");
		Menu3 = wx.Menu();
		MenuItem12 = wx.Menu();
		MenuItem9 = wx.MenuItem(MenuItem12, ID_MENUITEM1, "&Alpha-Beta Pruning", wx.EmptyString, wx.ITEM_CHECK);
		MenuItem12.Append(MenuItem9);
		MenuItem9.Enable(False);
		MenuItem9.Check(True);
		MenuItem13 = wx.MenuItem(MenuItem12, ID_MENUITEM2, "&Principal Variation Search", wx.EmptyString, wx.ITEM_CHECK);
		MenuItem12.Append(MenuItem13);
		MenuItem13.Enable(False);
		MenuItem14 = wx.MenuItem(MenuItem12, ID_MENUITEM3, "&Transition Table", wx.EmptyString, wx.ITEM_CHECK);
		MenuItem12.Append(MenuItem14);
		MenuItem14.Enable(False);
		MenuItem15 = wx.MenuItem(MenuItem12, ID_MENUITEM4, "&Killer Heuristic", wx.EmptyString, wx.ITEM_CHECK);
		MenuItem12.Append(MenuItem15);
		MenuItem15.Enable(False);
		MenuItem15.Check(True);
		MenuItem16 = wx.MenuItem(MenuItem12, ID_MENUITEM5, "&Memory-Enhanced Test Driver", wx.EmptyString, wx.ITEM_CHECK);
		MenuItem12.Append(MenuItem16);
		MenuItem16.Enable(False);
		MenuItem17 = wx.MenuItem(MenuItem12, ID_MENUITEM6, "&Iterative Deepening Search", wx.EmptyString, wx.ITEM_CHECK);
		MenuItem12.Append(MenuItem17);
		MenuItem17.Enable(False);
		Menu3.Append(id_algorithm, "&Algorithm", MenuItem12, wx.EmptyString);
		MenuItem6 = wx.MenuItem(Menu3, id_level, "&Level", wx.EmptyString, wx.ITEM_NORMAL);
		Menu3.Append(MenuItem6);
		MenuBar1.Append(Menu3, "&Settings");
		Menu2 = wx.Menu();
		MenuItem2 = wx.MenuItem(Menu2, id_about, "&About\tF1", "Show info about this application", wx.ITEM_NORMAL);
		Menu2.Append(MenuItem2);
		MenuBar1.Append(Menu2, "&Help");
		self.SetMenuBar(MenuBar1);
		StatusBar1 = wx.StatusBar(self, ID_STATUSBAR1, 0, "ID_STATUSBAR1");
		StatusBar1.SetFieldsCount(1,(-1,));
		StatusBar1.SetStatusStyles((wx.SB_NORMAL,));
		self.SetStatusBar(StatusBar1);
		self.dialog_load = wx.FileDialog(self, "Select file", wx.EmptyString, wx.EmptyString, "*.tcl", wx.FD_DEFAULT_STYLE, wx.DefaultPosition, wx.DefaultSize, "wxFileDialog");
		box_sizer_base.Fit(self);
		box_sizer_base.SetSizeHints(self);
		
		Notebook1.SetPageText(0,"terminal");
		self.text_log = wx.TextCtrl(Notebook1, id_text_log, wx.EmptyString, wx.Point(-81,23), wx.Size(283,296), wx.TE_MULTILINE|wx.TE_READONLY, wx.DefaultValidator, "id_text_log");
		self.text_log.SetForegroundColour(wx.Colour(200,200,200));
		self.text_log.SetBackgroundColour(wx.Colour(32,32,32));
		text_log_font = wx.Font(14,wx.FONTFAMILY_SWISS,wx.FONTSTYLE_NORMAL,wx.FONTWEIGHT_BOLD,False,"Consolas",wx.FONTENCODING_DEFAULT);
		self.text_log.SetFont(text_log_font);
		Notebook1.AddPage(self.text_log, "log", False);
		book_tree = wx.TreeCtrl(Notebook1, id_book_tree, wx.DefaultPosition, wx.Size(351,230), wx.TR_DEFAULT_STYLE, wx.DefaultValidator, "id_book_tree");
		book_tree.SetForegroundColour(wx.Colour(200,200,200));
		book_tree.SetBackgroundColour(wx.Colour(32,32,32));
		book_tree.SetFont(text_log_font);
		Notebook1.AddPage(book_tree, "book", False);
		
		self.panel_board.Bind(wx.EVT_PAINT,self.on_panel_board_paint);
		self.panel_board.Bind(wx.EVT_LEFT_DOWN,self.on_panel_board_leftdown);
		self.text_input.Bind(wx.EVT_TEXT_ENTER,self.on_text_input_textenter);
		self.Bind(wx.EVT_MENU,self.on_quit,id = id_quit);
		self.Bind(wx.EVT_MENU,self.on_about,id = id_about);
		self.Bind(wx.EVT_MENU,self.on_black,id = id_player_black);
		self.Bind(wx.EVT_MENU,self.on_white,id = id_player_white);
		self.Bind(wx.EVT_MENU,self.on_undo,id = id_undo);
		self.Bind(wx.EVT_MENU,self.on_redo,id = id_redo);
		self.Bind(wx.EVT_MENU,self.on_load,id = id_load);
		self.Bind(wx.EVT_MENU,self.on_clear_log,id = id_clear_log);
		self.Bind(wx.EVT_MENU,self.on_clear_term,id = id_clear_term);
		self.Bind(wx.EVT_MENU,self.on_clear_all,id = id_clear_all);
		self.Bind(wx.EVT_MENU,self.on_clockwise,id = id_clockwise);
		self.Bind(wx.EVT_MENU,self.on_counterclockwise,id = id_counterclockwise);
		self.Bind(wx.EVT_MENU,self.on_horizontal,id = id_horizontal);
		self.Bind(wx.EVT_MENU,self.on_vertical,id = id_vertical);
		self.Bind(wx.EVT_MENU,self.on_reflect,id = id_reflect);
		# Connect(id_book_tree,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,(wxObjectEventFunction)&reversi_guiFrame::on_tree_item_select);
		
		# self.panel_board.Connect(wxEVT_CONTEXT_MENU,wxContextMenuEventHandler(reversi_guiFrame::on_context_menu),NULL,this); 
	
	# mygame = rv.game();
	def on_quit(self,event):
		self.Close();
	def on_about(self,event):
		wx.MessageBox("Reversi Game\nBy Rick\n1300012743@pku.edu.cn","About");
	def on_panel_board_paint(self, event):
		self.paint();
	def paint(self):
		bias = 10;
		num = 8;
		cell = 50;
		width = num * cell;
		cbias = bias + cell / 2;
		radius = cell / 2 - 5;

		dc = wx.ClientDC(self.panel_board);
		dc.Clear();
		
		#draw valid moves
		dc.SetBrush(wx.Brush(wx.Colour(23,95,0)));
		dc.SetPen(wx.Pen(wx.Colour(23,95,0),4));
		brd_move = mygame.brd.get_move(mygame.color);
		for i in range(rv.board.size2):
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
				chssmn = mygame.brd.get(i + (j << 3));
				if chssmn == rv.black:
					dc.SetBrush(wx.Brush(wx.Colour(40,40,40)));
					dc.SetPen(wx.Pen(wx.Colour(20,20,20),4));
					dc.DrawCircle(wx.Point(cbias + cell * i,cbias + cell * j),radius);
				elif chssmn == rv.white:
					dc.SetBrush(wx.Brush(wx.Colour(210,210,210)));
					dc.SetPen(wx.Pen(wx.Colour(230,230,230),4));
					dc.DrawCircle(wx.Point(cbias + cell * i,cbias + cell * j),radius);
		
		#show where is the last move
		dc.SetBrush(wx.TRANSPARENT_BRUSH);
		dc.SetPen(wx.Pen(wx.YELLOW,4));
		dc.DrawCircle(wx.Point(cbias + cell * mygame.pos.x,cbias + cell * mygame.pos.y),radius);
	
	def _print(self,s):
		self.text_terminal.AppendText(str(s) + "\n");
	def process(self,s):
		self.text_terminal.AppendText(">>" + s + "\n");
		exec(s);
		if mygame.flag_log:
			self.text_log.AppendText(mygame.log_string);
			mygame.flag_log = False;
		if mygame.flag_show:
			self.paint();
			mygame.flag_show = False;
	def on_text_input_textenter(self,event):
		self.process(self.text_input.GetValue());
	def on_black(self,event):
		self.process("mygame.start();");
	def on_white(self,event):
		self.process("mygame.start();");
		self.process(
			"result = mygame.play(rv.mthd_default,True);\n"
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
	def on_clockwise(self, event):
		self.process("mygame.rotate_r();");
	def on_counterclockwise(self, event):
		self.process("mygame.rotate_l();");
	def on_horizontal(self, event):
		self.process("mygame.mirror_h();");
	def on_vertical(self, event):
		self.process("mygame.mirror_v();");
	def on_reflect(self, event):
		self.process("mygame.reflect();");
	def on_clear_log(self, event):
		self.process("self.text_log.Clear();");
	def on_clear_term(self, event):
		self.process("self.text_terminal.Clear();");
	def on_clear_all(self, event):
		self.process("self.text_log.Clear();");
		self.process("self.text_terminal.Clear();");
	def on_panel_board_leftdown(self,event):
		bias = 10;
		cell = 50;
		# if mygame.is_lock:
			# return;
		pos = event.GetPosition();
		x = int((pos.x - bias) / cell);
		y = int((pos.y - bias) / cell);
		self.process(
			"result = mygame.play(rv.coordinate(" + str(x) + ","
			+ str(y) + "),rv.mthd_default);\n"
			+ "result = (result.x,result.y);\n"
			+ "self._print(result);"
		)

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

if __name__ == '__main__':
  
    app = wx.App()
    reversi_frame(None)
    app.MainLoop()