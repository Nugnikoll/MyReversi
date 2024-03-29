# -*- coding: utf-8 -*-

import wx
import sys
import os
import _thread
import time
import re
from IPython.terminal.embed import InteractiveShellEmbed as interactive_shell_embed
import IPython.core.page
from game import rv, game, bias, cell, data_path, image_path, default_path, mthd_default
from load_log import load_log

def page_printer(data, start = 0, screen_lines = 0, pager_cmd = None):
	if isinstance(data, dict):
		data = data['text/plain']
	print(data)
IPython.core.page.page = page_printer
ipshell = interactive_shell_embed(
	user_ns = globals(), colors = "Linux",
)

rv.board.config()
rv.pattern.config(data_path + "pattern.dat")

evt_thrd_id = wx.ID_ANY

os

class thrd_event(wx.PyEvent):
	def __init__(self,data):
		wx.PyEvent.__init__(self)
		self.SetEventType(evt_thrd_id)
		self.data = data

class panel_board(wx.Panel):
	def __init__(self, parent):
		wx.Panel.__init__(self, parent, size = wx.Size(424,424), style = wx.SUNKEN_BORDER | wx.TAB_TRAVERSAL)

		self.frame = parent.frame

		self.SetBackgroundColour(wx.Colour(43,155,0))

		self.Bind(wx.EVT_PAINT, self.on_paint)
		self.Bind(wx.EVT_LEFT_DOWN, self.on_leftdown)
	
	#paint on panel_board
	def on_paint(self, event):
		self.paint()

	#paint on panel_board
	def paint(self):
		mygame.show(dc = wx.ClientDC(self))
	
	#click on the panel board
	def on_leftdown(self, event):
		# if mygame.is_lock:
			# return
		pos = event.GetPosition()
		if pos.x < bias:
			x = -1
		else:
			x = int((pos.x - bias) / cell)
		if pos.y < bias:
			y = -1
		else:
			y = int((pos.y - bias) / cell)
		self.frame.process("mygame.click((%d,%d))" % (x,y))

class text_input(wx.TextCtrl):
	def __init__(self, parent):
		self.frame = parent.frame
		wx.TextCtrl.__init__(
			self, parent, size = wx.Size(266,30),
			style = wx.TE_PROCESS_ENTER | wx.TE_PROCESS_TAB | wx.TE_MULTILINE
		)
		self.SetForegroundColour(wx.Colour(200,200,200))
		self.SetBackgroundColour(wx.Colour(32,32,32))
		self.SetFont(self.frame.font_text)
		self.Bind(wx.EVT_TEXT_ENTER, self.on_textenter)
		self.Bind(wx.EVT_CHAR, self.on_char)

	#input command
	def on_textenter(self, event):
		self.frame.process(self.GetValue())

	#capture tab and enter
	def on_char(self, event):
		code = event.GetKeyCode()

		if code == 9:  # Tab
			value = self.GetValue()
			pos = self.GetInsertionPoint()
			coord = self.PositionToCoords(pos)
			hint = re.search(r"[_0-9a-zA-Z.%]*$", value[:pos]).group()
			pos_hint = pos - len(hint)
			hint += re.search(r"^[_0-9a-zA-Z.%]*", value[pos:]).group()
			if not len(hint):
				return
			complete_list = ipshell.complete(hint, cursor_pos = pos - pos_hint)
			if not len(complete_list[1]):
				return

			menu = wx.Menu()
			for item in complete_list[1]:
				menu_item = wx.MenuItem(menu, wx.ID_ANY, item)
				menu_item.data = item
				menu.Append(menu_item)
				def on_menu_item(event):
					child = menu.FindChildItem(event.GetId())
					v = value[:pos_hint]
					v += child[0].data
					v += value[pos_hint + len(hint):]
					self.SetValue(v)
					self.SetInsertionPoint(pos_hint + len(child[0].data))
				menu.Bind(wx.EVT_MENU, on_menu_item, menu_item)
			self.PopupMenu(menu, coord)

		elif code == 13: # Enter
			self.frame.process(self.GetValue())

		else:
			event.Skip()

class frame_main(wx.Frame):
	#initialize the frame
	def __init__(
		self, parent = None, id = wx.ID_ANY, title = "Reversi",
		pos = wx.DefaultPosition, size = wx.DefaultSize,
		style = wx.DEFAULT_FRAME_STYLE | wx.SUNKEN_BORDER | wx.CLIP_CHILDREN,
	):

		#create a frame
		wx.Frame.__init__(self, parent, id , title, pos, size, style)
		self.frame = self

		self.font_text = wx.Font(14, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD, faceName = "consolas")
		self.font_index = wx.Font(11, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD, faceName = "consolas")
		self.SetFont(self.font_text)
		self.SetFont(self.font_index)

		#set the icon of the frame
		frame_icon = wx.Icon()
		frame_icon.CopyFromBitmap(wx.Bitmap(wx.Image(image_path + "reversi.png")))
		self.SetIcon(frame_icon)

		#load images
		self.img_board = wx.Bitmap(wx.Image(image_path + "board/board.png"))
		self.img_cell = wx.Bitmap(wx.Image(image_path + "board/cell.png"))
		self.img_move = wx.Bitmap(wx.Image(image_path + "board/move.png"))
		self.img_stone = []
		str_color = ["white", "black"]
		str_light = ["", "_light"]
		str_move = ["", "move_"]
		for i in (False, True):
			self.img_stone.append([])
			for j in (False, True):
				self.img_stone[i].append(wx.Bitmap(wx.Image(
					image_path + "board/stone_%s%s.png" % (str_color[i], str_light[j])
				)))
		self.img_pvs = []
		for i in (False, True):
			self.img_pvs.append([])
			for j in (False, True):
				self.img_pvs[i].append([])
				for k in range(64):
					self.img_pvs[i][j].append(wx.Bitmap(wx.Image(
						image_path + "board/pvs_%s%s_%s.png" % (str_move[i], str_color[j], str(k))
					)))

		#create background elements
		sizer_base = wx.BoxSizer(wx.HORIZONTAL)
		self.SetSizer(sizer_base)
		panel_base = wx.Panel(self)
		panel_base.frame = self
		panel_base.SetBackgroundColour(wx.BLACK)
		sizer_base.Add(panel_base, 1, wx.ALL | wx.EXPAND,5)
		sizer_main = wx.BoxSizer(wx.HORIZONTAL)
		panel_base.SetSizer(sizer_main)

		#create a panel for drawing chess board on the left
		sizer_board = wx.BoxSizer(wx.VERTICAL)
		sizer_main.Add(sizer_board, 0, wx.ALL | wx.ALIGN_CENTER, 5)
		self.panel_board = panel_board(panel_base)
		sizer_board.Add(self.panel_board, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		#create a sizer for adding elements on the right
		sizer_tool = wx.BoxSizer(wx.VERTICAL)
		sizer_main.Add(sizer_tool, 1, wx.ALL | wx.EXPAND, 5)

		#create a label on the top right
		text_label = wx.StaticText(panel_base, label = "Reversi")
		sizer_tool.Add(text_label, 0, wx.ALL | wx.ALIGN_CENTER, 5)
		text_label.SetForegroundColour(wx.Colour(200,200,200))
		font_text_label = wx.Font(26, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD, faceName = "Segoe Script")
		text_label.SetFont(font_text_label)

		#create a notebook on the right
		notebook = wx.Notebook(panel_base, size = wx.Size(448,296), style = wx.TAB_TRAVERSAL)
		sizer_tool.Add(notebook, 1, wx.ALL | wx.EXPAND, 5)
		notebook.SetFont(self.font_index)

		#add interaction page to the notebook
		panel_note = wx.Panel(notebook, style =  wx.TAB_TRAVERSAL)
		notebook.AddPage(panel_note, "Interaction")
		panel_note.SetForegroundColour(wx.Colour(200,200,200))
		panel_note.SetBackgroundColour(wx.Colour(32,32,32))
		self.sizer_note = wx.BoxSizer(wx.VERTICAL)
		panel_note.SetSizer(self.sizer_note)
		panel_note.SetFont(self.font_text)

		#specify black player in the interaction page
		self.sizer_note_black = wx.BoxSizer(wx.HORIZONTAL)
		self.sizer_note.Add(self.sizer_note_black, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		label_black = wx.StaticText(panel_note, label = "Black Player")
		self.sizer_note_black.Add(label_black, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		self.choice_black = wx.Choice(panel_note)
		self.choice_black.SetSelection(self.choice_black.Append("Human"))
		self.choice_black.Append("Computer")
		self.choice_black.Append("Other Program")
		self.choice_black.SetForegroundColour(wx.Colour(32,32,32))
		self.choice_black.SetBackgroundColour(wx.Colour(200,200,200))
		self.sizer_note_black.Add(self.choice_black, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		#specify black player path in the interaction page
		self.sizer_note_path_black = wx.BoxSizer(wx.HORIZONTAL)
		self.sizer_note.Add(self.sizer_note_path_black, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		self.text_path_black = wx.TextCtrl(
			panel_note, value = default_path,
			size = (332,30), style = wx.TE_PROCESS_ENTER
		)
		self.text_path_black.SetBackgroundColour(wx.Colour(200,200,200))
		self.sizer_note_path_black.Add(self.text_path_black, 1, wx.ALL | wx.ALIGN_CENTER, 5)
		self.text_path_black.Hide()

		self.button_folder_black = wx.BitmapButton(
			panel_note, bitmap = wx.Bitmap(wx.Image(image_path + "folder_small.png")), size = wx.Size(32,29)
		)
		self.button_folder_black.SetBackgroundColour(wx.Colour(0,0,0))
		self.sizer_note_path_black.Add(self.button_folder_black, 0, wx.ALL | wx.ALIGN_CENTER, 5)
		self.button_folder_black.Hide()

		#specify white player in the interaction page
		self.sizer_note_white = wx.BoxSizer(wx.HORIZONTAL)
		self.sizer_note.Add(self.sizer_note_white, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		label_white = wx.StaticText(panel_note, label = "White Player")
		self.sizer_note_white.Add(label_white, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		self.choice_white = wx.Choice(panel_note)
		self.choice_white.Append("Human")
		self.choice_white.SetSelection(self.choice_white.Append("Computer"))
		self.choice_white.Append("Other Program")
		self.choice_white.SetForegroundColour(wx.Colour(32,32,32))
		self.choice_white.SetBackgroundColour(wx.Colour(200,200,200))
		self.sizer_note_white.Add(self.choice_white, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		#specify white player path in the interaction page
		self.sizer_note_path_white = wx.BoxSizer(wx.HORIZONTAL)
		self.sizer_note.Add(self.sizer_note_path_white, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		self.text_path_white = wx.TextCtrl(
			panel_note, value = default_path,
			size = (332,30), style = wx.TE_PROCESS_ENTER
		)
		self.text_path_white.SetBackgroundColour(wx.Colour(200,200,200))
		self.sizer_note_path_white.Add(self.text_path_white, 1, wx.ALL | wx.ALIGN_CENTER, 5)
		self.text_path_white.Hide()

		self.button_folder_white = wx.BitmapButton(
			panel_note, bitmap = wx.Bitmap(wx.Image(image_path + "folder_small.png")), size = wx.Size(32,29)
		)
		self.button_folder_white.SetBackgroundColour(wx.Colour(0,0,0))
		self.sizer_note_path_white.Add(self.button_folder_white, 0, wx.ALL | wx.ALIGN_CENTER, 5)
		self.button_folder_white.Hide()

		#add start button to the interaction page
		self.sizer_note_start = wx.BoxSizer(wx.HORIZONTAL)
		self.sizer_note.Add(self.sizer_note_start, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		self.button_start = wx.Button(panel_note, label = "Start a New Game")
		#self.button_start.SetForegroundColour(wx.Colour(32,32,32))
		#self.button_start.SetBackgroundColour(wx.Colour(200,200,200))
		self.sizer_note_start.Add(self.button_start, 1, wx.ALL | wx.ALIGN_CENTER, 5)

		#add terminal page to the notebook
		self.text_term = wx.TextCtrl(
			notebook, style = wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_RICH2
		)
		notebook.AddPage(self.text_term, "Terminal")
		self.text_term.SetForegroundColour(wx.Colour(200,200,200))
		self.text_term.SetBackgroundColour(wx.Colour(32,32,32))
		self.text_term.SetFont(self.font_text)
		self.frame.text_term.SetDefaultStyle(wx.TextAttr(wx.Colour(200,200,200)))

		#add log page to the notebook
		self.text_log = wx.TextCtrl(notebook, style = wx.TE_MULTILINE | wx.TE_READONLY)
		notebook.AddPage(self.text_log, "Log")
		self.text_log.SetForegroundColour(wx.Colour(200,200,200))
		self.text_log.SetBackgroundColour(wx.Colour(32,32,32))
		self.text_log.SetFont(self.font_text)

		self.tree_list = wx.TreeCtrl(notebook)
		notebook.AddPage(self.tree_list, "Tree")
		self.tree_list.SetForegroundColour(wx.Colour(200,200,200))
		self.tree_list.SetBackgroundColour(wx.Colour(32,32,32))
		self.tree_list.SetFont(self.font_text)
		self.tree_list.Hide()

		#create a input box on the bottom right
		self.text_input = text_input(panel_base)
		sizer_tool.Add(self.text_input, 0, wx.ALL | wx.EXPAND, 5)

		#create a menu bar
		self.menubar = wx.MenuBar()
		self.SetMenuBar(self.menubar)
		menu_file = wx.Menu()
		self.menubar.Append(menu_file, "&File")
		menu_edit = wx.Menu()
		self.menubar.Append(menu_edit, "&Edit")
		menu_analyze = wx.Menu()
		self.menubar.Append(menu_analyze, "&Analyze")
		menu_setting = wx.Menu()
		self.menubar.Append(menu_setting, "&Setting")
		menu_help = wx.Menu()
		self.menubar.Append(menu_help, "&Help")

		#add items to menu_file
		menu_new = wx.MenuItem(
			menu_file, id = wx.ID_ANY,
			text = "&New Game\tCtrl-N",
			helpString = "Start a new game"
		)
		menu_file.Append(menu_new)
		menu_load = wx.MenuItem(
			menu_file, id = wx.ID_ANY,
			text = "&Load Script\tCtrl-L",
			helpString = "Load and execute a Python script"
		)
		menu_file.Append(menu_load)
		menu_export = wx.MenuItem(
			menu_file, id = wx.ID_ANY,
			text = "&Export Image\tCtrl-S",
			helpString = "Export the board as an image"
		)
		menu_file.Append(menu_export)
		menu_quit = wx.MenuItem(
			menu_file, id = wx.ID_ANY,
			text = "&Quit\tAlt-F4",
			helpString = "Quit the application"
		)
		menu_file.Append(menu_quit)

		#add items to menu_edit
		menu_undo = wx.MenuItem(
			menu_edit, id = wx.ID_ANY,
			text = "&Undo\tCtrl+Z",
			helpString = "Undo the latest change"
		)
		menu_edit.Append(menu_undo)
		menu_redo = wx.MenuItem(
			menu_edit, id = wx.ID_ANY,
			text = "&Redo\tCtrl+Y",
			helpString = "Redo the latest change"
		)
		menu_edit.Append(menu_redo)
		menu_transform = wx.Menu()
		menu_edit.AppendSubMenu(menu_transform, "&Transform")
		menu_clear = wx.Menu()
		menu_edit.AppendSubMenu(menu_clear, "&Clear")

		#add items to menu_transform
		menu_mirror_h = wx.MenuItem(
			menu_transform, id = wx.ID_ANY,
			text = "Mirror &Horrizontally\tAlt+H"
		)
		menu_mirror_h.num = 0
		menu_transform.Append(menu_mirror_h)
		menu_mirror_v = wx.MenuItem(
			menu_transform, id = wx.ID_ANY,
			text = "Mirror &Vertically\tAlt+V"
		)
		menu_mirror_v.num = 1
		menu_transform.Append(menu_mirror_v)
		menu_reflect = wx.MenuItem(
			menu_transform, id = wx.ID_ANY,
			text = "&Reflect\tAlt+R"
		)
		menu_reflect.num = 2
		menu_transform.Append(menu_reflect)
		menu_rotate_r = wx.MenuItem(
			menu_transform, id = wx.ID_ANY,
			text = "Rotate &Clockwise\tAlt+C"
		)
		menu_rotate_r.num = 3
		menu_transform.Append(menu_rotate_r)
		menu_rotate_l = wx.MenuItem(
			menu_transform, id = wx.ID_ANY,
			text = "Rotate Coun&terclockwise\tAlt+T"
		)
		menu_rotate_l.num = 4
		menu_transform.Append(menu_rotate_l)
		menu_reverse = wx.MenuItem(
			menu_transform, id = wx.ID_ANY,
			text = "Rever&se\tAlt+S"
		)
		menu_reverse.num = 5
		menu_transform.Append(menu_reverse)

		#add items to menu_clear
		menu_clear_term = wx.MenuItem(
			menu_clear, id = wx.ID_ANY,
			text = "Clear Terminal"
		)
		menu_clear.Append(menu_clear_term)
		menu_clear_log = wx.MenuItem(
			menu_clear, id = wx.ID_ANY,
			text = "Clear Log"
		)
		menu_clear.Append(menu_clear_log)

		#add items to menu_analyze
		menu_search = wx.MenuItem(
			menu_analyze, id = wx.ID_ANY,
			text = "Search\tCtrl+A",
		)
		menu_analyze.Append(menu_search)
		menu_choice = wx.MenuItem(
			menu_analyze, id = wx.ID_ANY,
			text = "Show Choices\tCtrl+E",
		)
		menu_analyze.Append(menu_choice)
		menu_pv = wx.MenuItem(
			menu_analyze, id = wx.ID_ANY,
			text = "Show &Principle Variation\tCtrl+P",
		)
		menu_analyze.Append(menu_pv)

		#add items to menu_setting
		self.menu_algorithm = wx.Menu()
		menu_setting.AppendSubMenu(self.menu_algorithm, "&Algorithm")
		self.menu_level = wx.Menu()
		menu_setting.AppendSubMenu(self.menu_level, "&Level")

		#add items to menu_algorithm
		alg_table = [
			rv.mthd_rnd, rv.mthd_ab, rv.mthd_pvs, rv.mthd_trans,
			rv.mthd_kill, rv.mthd_mtdf, rv.mthd_ids, rv.mthd_ptn,
			rv.mthd_mpc
		]
		alg_str_table = [
			"rv.mthd_rnd", "rv.mthd_ab", "rv.mthd_pvs", "rv.mthd_trans",
			"rv.mthd_kill", "rv.mthd_mtdf", "rv.mthd_ids", "rv.mthd_ptn",
			"rv.mthd_mpc"
		]
		alg_text_table = [
			"&Random", "&Alpha-Beta Pruning", "Principle &Variation Search", "&Transposition Table",
			"&Killer Heuristic", "&Memory Enhanced Test Driver", "&Iterative Deepening Search", "&Pattern",
			"Multi-Probability &Cut"
		]
		for i in range(len(alg_table)):
			menu_alg = wx.MenuItem(
				self.menu_algorithm, id = wx.ID_ANY,
				text = alg_text_table[i],
				kind = wx.ITEM_CHECK
			)
			menu_alg.mthd = alg_table[i]
			menu_alg.mthd_str = alg_str_table[i]
			self.menu_algorithm.Append(menu_alg)
			if alg_table[i] & mthd_default:
				menu_alg.Check(True)
		self.menu_algorithm.Insert(1, wx.ID_ANY, kind = wx.ITEM_SEPARATOR)

		#add items to menu_level
		for i in range(10):
			self.menu_level.Append(
				wx.MenuItem(
					self.menu_level, id = wx.ID_ANY,
					text = "Level %d" % (i + 1),
					kind = wx.ITEM_CHECK
				)
			)
		self.menu_level.GetMenuItems()[7].Check(True)

		#add items to menu_help
		menu_about = wx.MenuItem(
			menu_help, id = wx.ID_ANY,
			text = "&About\tF1",
			helpString = "Show info about this application"
		)
		menu_help.Append(menu_about)

		#create a status bar
		self.statusbar = wx.StatusBar(self)
		self.statusbar.SetFieldsCount(3, (-1, -1, -1))
		self.statusbar.SetStatusStyles((wx.SB_NORMAL, wx.SB_NORMAL, wx.SB_NORMAL))
		self.SetStatusBar(self.statusbar)

		#set the suitable size of the frame
		sizer_base.SetSizeHints(self)

		#bind interaction events
		self.choice_black.Bind(wx.EVT_CHOICE, self.on_choice_player)
		self.text_path_black.Bind(wx.EVT_TEXT_ENTER, self.on_text_path_enter)
		self.button_folder_black.Bind(wx.EVT_BUTTON, self.on_button_folder_click)
		self.choice_white.Bind(wx.EVT_CHOICE, self.on_choice_player)
		self.text_path_white.Bind(wx.EVT_TEXT_ENTER, self.on_text_path_enter)
		self.button_folder_white.Bind(wx.EVT_BUTTON, self.on_button_folder_click)
		self.tree_list.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.on_tree_item_select)
		self.button_start.Bind(wx.EVT_BUTTON, self.on_start)
		self.Bind(wx.EVT_MENU, self.on_start, id = menu_new.GetId())
		self.Bind(wx.EVT_MENU, self.on_load, id = menu_load.GetId())
		self.Bind(wx.EVT_MENU, self.on_export, id = menu_export.GetId())
		self.Bind(wx.EVT_MENU, self.on_quit, id = menu_quit.GetId())
		self.Bind(wx.EVT_MENU, self.on_undo, id = menu_undo.GetId())
		self.Bind(wx.EVT_MENU, self.on_redo, id = menu_redo.GetId())
		self.Bind(wx.EVT_MENU, self.on_search, id = menu_search.GetId())
		self.Bind(wx.EVT_MENU, self.on_choice, id = menu_choice.GetId())
		self.Bind(wx.EVT_MENU, self.on_pv, id = menu_pv.GetId())
		self.Bind(wx.EVT_MENU, self.on_clear_log, id = menu_clear_log.GetId())
		self.Bind(wx.EVT_MENU, self.on_clear_term, id = menu_clear_term.GetId())
		self.Bind(wx.EVT_MENU, self.on_about, id = menu_about.GetId())

		for i in menu_transform.GetMenuItems():
			self.Bind(wx.EVT_MENU, self.on_menu_trans, i)

		for i in self.menu_algorithm.GetMenuItems():
			self.Bind(wx.EVT_MENU, self.on_menu_alg, i)

		for i in self.menu_level.GetMenuItems():
			self.Bind(wx.EVT_MENU, self.on_menu_level, i)

		self.Connect(-1, -1, evt_thrd_id, self.thrd_catch)

		#redirect iostream
		self.stdout_save = sys.stdout
		self.stderr_save = sys.stderr
		
		class redirect:
			frame = self
			color_map = {
				"\033[0;30m": wx.Colour(46,52,54),
				"\033[1;30m": wx.Colour(85,87,83),
				"\033[0;31m": wx.Colour(204,0,0),
				"\033[1;31m": wx.Colour(239,41,41),
				"\033[0;32m": wx.Colour(78,154,6),
				"\033[1;32m": wx.Colour(138,226,52),
				"\033[0;33m": wx.Colour(196,160,0),
				"\033[1;33m": wx.Colour(252,233,79),
				"\033[0;34m": wx.Colour(52,101,164),
				"\033[1;34m": wx.Colour(114,159,207),
				"\033[0;35m": wx.Colour(117,80,123),
				"\033[1;35m": wx.Colour(173,127,168),
				"\033[0;36m": wx.Colour(6,152,154),
				"\033[1;36m": wx.Colour(52,226,226),
				"\033[0;37m": wx.Colour(211,215,207),
				"\033[1;37m": wx.Colour(238,238,236),
			}

			def write(self, s):
				ptn_color = r"\x1b\[[01](?:;[0-9]+)?m"
				g = re.finditer(ptn_color, s)
				pos1 = 0
				for i in g:
					self.frame.text_term.AppendText(s[pos1:i.span()[0]])
					color = i.group()
					if color in self.color_map:
						color = self.color_map[color]
						self.frame.text_term.SetDefaultStyle(wx.TextAttr(color))
					else:
						self.frame.text_term.SetDefaultStyle(wx.TextAttr(wx.Colour(200,200,200)))
					pos1 = i.span()[1]
				self.frame.text_term.AppendText(s[pos1:])
			def flush(self):
				pass

		self.obj_redirect = redirect()

		sys.stdout = self.obj_redirect
		sys.stderr = self.obj_redirect

		#show the frame
		self.Show(True)
		self.tree_list.Hide()

		self.thrd_lock = False

	def print_console(self, s):
		self.stdout_save.write(s)

	#process command
	def process(self, s):
		if self.thrd_lock:
			return
		print("\033[1;32m>>\033[0m" + s)

		if s[0] == "%":
			time_start = time.time()
			result = ipshell.magic(s)
			if result:
				print(result)
			time_end = time.time()
		else:
			query = re.search(r"[_0-9a-zA-Z.]+\?", s)
			if query:
				time_start = time.time()
				result = ipshell.magic("pinfo " + query.group()[:-1])
				time_end = time.time()
			else:
				query = re.search(r"[_0-9a-zA-Z.]+\?\?", s)
				if query:
					time_start = time.time()
					result = ipshell.magic("pinfo2 " + query.group()[:-1])
					time_end = time.time()
				time_start = time.time()
				ipshell.ex(s)
				time_end = time.time()

		self.statusbar.SetStatusText("Wall time : %f seconds" % (time_end - time_start), 2)

	def on_quit(self, event):
		self.Close()

	def on_about(self, event):
		wx.MessageBox("Reversi Game\nBy Rick", "About")

	#export image
	def on_export(self, event):
		dialog_export = wx.FileDialog(self, message = "Export image", wildcard = "*.png", style = wx.FD_SAVE)

		if dialog_export.ShowModal() == wx.ID_OK:
			path = dialog_export.GetPath()
			path = path.replace("\\","\\\\")
			self.process("mygame.export(\"" + path + "\", img_type = wx.BITMAP_TYPE_PNG)\n")

	#load and execute a script
	def load_script(self, s):
		fobj = open(s)
		lines = fobj.readlines()
		fobj.close()
		lines = "\n".join(lines)
		ipshell.ex(s)
		return True

	#load and execute a script
	def on_load(self, event):
		dialog_load = wx.FileDialog(self, message = "Load a python script", wildcard = "*.py", style = wx.FD_OPEN)

		if dialog_load.ShowModal() == wx.ID_OK:
			path = dialog_load.GetPath()
			path = path.replace("\\","\\\\")
			self.process("frame.load_script(\"" + path + "\")\n")

	def on_choice_player(self, event):
		if event.GetId() == self.choice_black.GetId():
			self.process(
				"mygame.ply[True].type = "
				+ str(self.choice_black.GetCurrentSelection())
				+ ""
			)

			if self.choice_black.GetCurrentSelection() == rv.ply_other:
				self.text_path_black.Show()
				self.button_folder_black.Show()
			else:
				self.text_path_black.Hide()
				self.button_folder_black.Hide()

			self.sizer_note.Layout()

		elif event.GetId() == self.choice_white.GetId():
			self.process(
				"mygame.ply[False].type = "
				+ str(self.choice_white.GetCurrentSelection())
				+ ""
			)

			if self.choice_white.GetCurrentSelection() == rv.ply_other:
				self.text_path_white.Show()
				self.button_folder_white.Show()
			else:
				self.text_path_white.Hide()
				self.button_folder_white.Hide()

			self.sizer_note.Layout()

	def on_text_path_enter(self, event):
		if event.GetId() == self.text_path_black.GetId():
			self.process(
				"mygame.ply[True].path = \""
				+ self.text_path_black.GetValue().replace("\\","\\\\")
				+ "\""
			)
		elif event.GetId() == self.text_path_white.GetId():
			self.process(
				"mygame.ply[False].path = \""
				+ self.text_path_white.GetValue().replace("\\","\\\\")
				+ "\""
			)

	def on_button_folder_click(self, event):
		if sys.platform == "win32":
			wildcard = "*.exe"
		else:
			wildcard = "*"

		if event.GetId() == self.button_folder_black.GetId():
			dialog_choice_player = wx.FileDialog(
				self, "Select file", wx.EmptyString, wx.EmptyString,
				wildcard, wx.FD_DEFAULT_STYLE, wx.DefaultPosition,
				wx.DefaultSize, "wxFileDialog"
			)

			if dialog_choice_player.ShowModal() == wx.ID_OK:
				path = dialog_choice_player.GetPath()
				self.text_path_black.SetValue(path)
				path = path.replace("\\","\\\\")
				self.process(
					"mygame.ply[True].path = \""
					+ path
					+ "\""
				)
		elif event.GetId() == self.button_folder_white.GetId():
			dialog_choice_player = wx.FileDialog(
				self, "Select file", wx.EmptyString, wx.EmptyString,
				wildcard, wx.FD_DEFAULT_STYLE, wx.DefaultPosition,
				wx.DefaultSize, "wxFileDialog"
			)

			if dialog_choice_player.ShowModal() == wx.ID_OK:
				path = dialog_choice_player.GetPath()
				self.text_path_white.SetValue(path)
				path = path.replace("\\","\\\\")
				self.process(
					"mygame.ply[False].path = \""
					+ path
					+ "\""
				)

	#start a new game
	def on_start(self, event):
		self.process("mygame.start()")

	def on_undo(self, event):
		self.process("mygame.undo()")

	def on_redo(self, event):
		self.process("mygame.redo()")

	def on_menu_trans(self, event):
		num = self.menubar.FindItemById(event.GetId()).num
		if num == 0:
			self.process("mygame.mirror_h()")
		elif num == 1:
			self.process("mygame.mirror_v()")
		elif num == 2:
			self.process("mygame.reflect()")
		elif num == 3:
			self.process("mygame.rotate_r()")
		elif num == 4:
			self.process("mygame.rotate_l()")
		elif num == 5:
			self.process("mygame.reverse()")

	def on_search(self, event):
		self.process("value = mygame.search(mygame.mthd, mygame.color, mygame.depth)")
		self.process("print(value)")

	def on_choice(self, event):
		self.process("choices = mygame.get_choice(mygame.mthd, mygame.color, mygame.depth)")
		self.process("print(choices)")

	def on_pv(self, event):
		self.process("pv = mygame.get_pv(mygame.color)")
		self.process("print(pv)")

	def on_clear_log(self, event):
		self.process("frame.text_log.Clear()")

	def on_clear_term(self, event):
		self.process("frame.text_term.Clear()")

	def on_clear_all(self, event):
		self.process("frame.text_log.Clear()")
		self.process("frame.text_term.Clear()")

	def on_menu_alg(self, event):
		id = event.GetId()
		item = self.menu_algorithm.FindItemById(id)

		if item.mthd == rv.mthd_rnd:
			if self.menu_algorithm.GetMenuItems()[0].IsChecked():
				for ptr in self.menu_algorithm.GetMenuItems():
					if ptr.IsCheckable():
						ptr.Check(False)
				self.process("mygame.mthd = rv.mthd_rnd")
			self.menu_algorithm.GetMenuItems()[0].Check(True)
		else:
			if item.IsChecked():
				self.menu_algorithm.GetMenuItems()[0].Check(False)
				self.process("mygame.mthd |= " + item.mthd_str + "")
			else:
				flag = False
				for ptr in self.menu_algorithm.GetMenuItems():
					if ptr.IsCheckable() and ptr.IsChecked():
						flag = True
				if not flag:
					self.menu_algorithm.GetMenuItems()[0].Check(True)

				self.process("mygame.mthd &= ~" + item.mthd_str + "")

	def on_menu_level(self, event):
		pos = 0
		for i in self.menu_level.GetMenuItems():
			i.Check(False)
		(item, pos) = self.menu_level.FindChildItem(event.GetId())
		item.Check(True)
		if pos >= 7:
			pos = 6 - pos
		else:
			pos = pos + 1

		self.process("mygame.depth = %d" % pos)

	def thrd_launch(self,fun,param):
		self.thrd_lock = True
		try:
			_thread.start_new_thread(self.thrd_wrap,(fun,param))
		except:
			print("fail to launch the thread!")
			self.thrd_lock = False

	def thrd_catch(self, event):
		self.thrd_lock = False

	def thrd_wrap(self, fun, param):
		try:
			fun(*param)
		except:
			print("fail to launch the thread!")
			self.thrd_lock = False
		wx.PostEvent(self, thrd_event(None))

	def sleep(self, count):
		time.sleep(count)
		print("sleep for %d seconds" % count)

	def on_tree_item_select(self, event):
		item = event.GetItem()
		(ptr, flag) = self.tree_list.GetItemData(item)
		if not flag:
			p = ptr.child
			while p:
				self.tree_list.AppendItem(item, p.info(), data = (p, False))
				p = p.sibling
			self.tree_list.SetItemData(item, (ptr, True))
		
		if hasattr(ptr, "brd") and not (ptr.brd is None):
			mygame.assign(rv.board(ptr.brd))
			if mygame.pos[0] >= 0:
				mygame.pos = (-1, -1)
			if hasattr(ptr, "color"):
				mygame.color = bool(ptr.color)
			mygame.show()

	def log_display(self, name):
		self.tree_list.DeleteAllItems()
		self.tree_list.Show()
		self.tree = load_log(name)
		ptr = self.tree.root
		self.tree_list.AddRoot(ptr.info(), data = (ptr, False))

	def tree_display(self, name):
		self.tree_list.DeleteAllItems()
		self.tree_list.Show()
		self.tree = rv.node()
		self.tree.load(name)
		self.tree_list.AddRoot(self.tree.info(), data = (self.tree, False))

class app_reversi(wx.App):
	#overload the initializer
	def OnInit(self):
		self.frame = frame_main()
		return True

if __name__ == "__main__":
	app = app_reversi(False)
	frame = app.frame
	mygame = game(app)
	app.MainLoop()
