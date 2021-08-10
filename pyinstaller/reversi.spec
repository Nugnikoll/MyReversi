# -*- mode: python  coding: utf-8 -*-

# freeze the python application into a stand-alone executable
# https://www.pyinstaller.org/

# usage:
# pyinstaller -y reversi.spec

import sys
import os
import shutil

if sys.platform == "win32":
	dll_suffix = ".pyd"
	exe_suffix = ".exe"
else:
	dll_suffix = ".so"
	exe_suffix = ""

if not os.path.exists("./cmake_build_asm"):
	os.mkdir("./cmake_build_asm")
if not os.path.exists("./cmake_build_avx2"):
	os.mkdir("./cmake_build_avx2")

if sys.platform == "win32":
	os.chdir("./cmake_build_asm")
	os.system("cmake -G\"MinGW Makefiles\" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=asm -S ..\\..\\build -B .\\")
	os.system("make")
	shutil.move("_reversi.dll", "_reversi_asm.pyd")
	shutil.move("bot.exe", "bot_asm.exe")
	os.chdir("..")

	os.chdir("./cmake_build_avx2")
	os.system("cmake -G\"MinGW Makefiles\" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=avx2 -S ..\\..\\build -B .\\")
	os.system("make")
	shutil.move("_reversi.dll", "_reversi_avx2.pyd")
	shutil.move("bot.exe", "bot_avx2.exe")
	os.chdir("../")
else:
	os.chdir("./cmake_build_asm")
	os.system("cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=asm -S ../../build -B ./")
	os.system("make")
	shutil.move("_reversi.so", "_reversi_asm.so")
	shutil.move("bot", "bot_asm")
	os.chdir("..")

	os.chdir("./cmake_build_avx2")
	os.system("cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_FORCE_ASM=avx2 -S ../../build -B ./")
	os.system("make")
	shutil.move("_reversi.so", "_reversi_avx2.so")
	shutil.move("bot", "bot_avx2")
	os.chdir("../")

block_cipher = None

mingw_dll = []
if sys.platform == "win32":
	env_path = os.environ["PATH"].split(";")
	mingw_path = None
	for path in env_path:
		if os.path.exists(path + "\\g++.exe"):
			mingw_path = path + "\\"
			break
	print("mingw_path", mingw_path)
	if mingw_path:
		mingw_dll = [(mingw_path + x, "./") for x in ["libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll"]]

a = Analysis(
	["../app/main.py"],
	pathex = ["./"],
	binaries = [
		("../python/reversi.py", "."),
		("./cmake_build_asm/cpuid*", "./"),
		("./cmake_build_asm/*" + dll_suffix, "./lib"),
		("./cmake_build_asm/bot*", "./lib"),
		("./cmake_build_avx2/*" + dll_suffix, "./lib"),
		("./cmake_build_avx2/bot*", "./lib"),
	] + mingw_dll,
	datas = [
		("../image/*.png", "./image"),
		("../image/*.ico", "./image"),
		("../image/board/*", "./image/board"),
		("../data/*", "./data")
	],
	hiddenimports = ["numpy.core.multiarray"],
	hookspath = [],
	runtime_hooks = [],
	excludes = [
		"babel",
		"bcolz",
		"blosc",
		"bokeh",
		"boto3",
		"botocore",
		"bottleneck",
		"cryptography",
		"cytoolz",
		"docutils",
		"h5py",
		"jsonschema",
		"lxml",
		"matplotlib",
		"netCDF4",
		"netcdftime",
		"notebook",
		"pandas",
		"PyQt5",
		"scipy",
		"sphinx",
		"tornado",
		"msgpack",
		"nbconvert",
		"nbformat",
		"simplejson",
		"zmq",
		"zope",
	],
	win_no_prefer_redirects = False,
	win_private_assemblies = False,
	cipher = block_cipher,
	noarchive = False
)

pyz = PYZ(
	a.pure, a.zipped_data,
	cipher = block_cipher
)

exe = EXE(
	pyz,
	a.scripts,
	[],
	exclude_binaries = True,
	name = "reversi",
	debug = False,
	bootloader_ignore_signals = False,
	strip = False,
	upx = False,
	console = False,
	icon = "../image/reversi.ico"
)

coll = COLLECT(
	exe,
	a.binaries,
	a.zipfiles,
	a.datas,
	strip = False,
	upx = True,
	upx_exclude = [],
	name = "reversi"
)