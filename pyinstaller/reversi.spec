# -*- mode: python ; coding: utf-8 -*-

import sys;
import os;

if sys.platform == "win32":
	dll_suffix = ".pyd";
	exe_suffix = ".exe"
else:
	dll_suffix = ".so";
	exe_suffix = "";

block_cipher = None;

bots = [];
if os.path.isfile("../bot/reversi" + exe_suffix):
	bots.append(("../bot/reversi" + exe_suffix, "./bot"));
if os.path.isfile("../bot/iris" + exe_suffix):
	bots.append(("../bot/iris" + exe_suffix, "./bot"));
if os.path.isfile("../bot/impbwcore" + exe_suffix):
	bots.append(("../bot/impbwcore" + exe_suffix, "./bot"));

a = Analysis(
	["../app/main.py"],
	pathex = ["./"],
	binaries = [
		("../python/_reversi" + dll_suffix,"."),
		("../python/reversi.py", ".")
	] + bots,
	datas = [
		("../image/*.png", "./image"),
		("../image/board/*", "./image/board"),
		("../data/*", "./data")
	],
	hiddenimports = ["numpy.core.multiarray"],
	hookspath = [],
	runtime_hooks = [],
	excludes = [],
	win_no_prefer_redirects = False,
	win_private_assemblies = False,
	cipher = block_cipher,
	noarchive = False
);

pyz = PYZ(
	a.pure, a.zipped_data,
	cipher = block_cipher
);

exe = EXE(
	pyz,
	a.scripts,
	[],
	exclude_binaries = True,
	name = "reversi",
	debug = False,
	bootloader_ignore_signals = False,
	strip = False,
	upx = True,
	console = False
);

coll = COLLECT(
	exe,
	a.binaries,
	a.zipfiles,
	a.datas,
	strip = False,
	upx = True,
	upx_exclude = [],
	name = "reversi"
);
