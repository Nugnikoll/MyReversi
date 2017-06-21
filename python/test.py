# -*- coding: utf-8 -*-

import wx
from wx import xrc
import reversi as rv
import pdb

class reversi_app(wx.App):

	def OnInit(self):
		# pdb.set_trace();
		self.res = xrc.XmlResource("..\\wxsmith\\reversi_guiframe.xrc");
		self.init_frame();
		return True;

	def init_frame(self):
		self.frame = self.res.LoadFrame(None, "reversi_guiFrame");
		self.frame.Show();

if __name__ == "__main__":
	app = reversi_app(False);
	app.MainLoop();