# -*- mode: python ; coding: utf-8 -*-

import sys;
import os;
import shutil;

if sys.platform == "win32":
	dll_suffix = ".pyd";
	exe_suffix = ".exe"
else:
	dll_suffix = ".so";
	exe_suffix = "";

if not os.path.exists("./cmake_build"):
	os.mkdir("./cmake_build");

if sys.platform == "win32":
	os.chdir("./cmake_build");
	os.system("cmake -G\"MinGW Makefiles\" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=asm ..\\..\\build");
	os.system("make");
	shutil.move("_reversi.dll", "_reversi_asm.pyd");
	shutil.move("bot.exe", "bot_asm.exe");
	os.system("cmake -G\"MinGW Makefiles\" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=avx2 ..\\..\\build");
	os.system("make");
	shutil.move("_reversi.dll", "_reversi_avx2.pyd");
	shutil.move("bot.exe", "bot_avx2.exe");
	os.chdir("../")
else:
	os.chdir("./cmake_build");
	os.system("cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=asm ../../build");
	os.system("make");
	shutil.move("_reversi.so", "_reversi_asm.so");
	shutil.move("bot", "bot_asm");
	os.system("cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=avx2 ../../build");
	os.system("make");
	shutil.move("_reversi.so", "_reversi_avx2.so");
	shutil.move("bot", "bot_avx2");
	os.chdir("../")

block_cipher = None;

bots = [];

a = Analysis(
	["../app/main.py"],
	pathex = ["./"],
	binaries = [
		("../python/reversi.py", "."),
		("./cmake_build/*" + dll_suffix, "./lib"),
		("./cmake_build/bot*", "./lib"),
		("./cmake_build/cpuid*", "./")
	] + bots,
	datas = [
		("../image/*.png", "./image"),
		("../image/*.ico", "./image"),
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
	console = False,
	icon = "../image/reversi.ico"
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
