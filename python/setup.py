from distutils.core import setup, Extension

src_dir = "../";
src = ["type.cc","reversi.cc","flip.cc","search.cc","pattern.cc","tree.cc"];
src = [src_dir + x for x in src];
src += ["reversi_wrap.cxx"];

reversi_module = Extension(
	"_reversi",
	src,
	define_macros=[("_hypot", "hypot"),("MS_WIN64", None)],
	extra_compile_args=["-Ofast"],
);

setup(
	name = "reversi",
	version = "0.1",
	author      = "Rick Lee",
	description = "\"Simple reversi game\"",
	ext_modules = [reversi_module],
	py_modules = ["reversi"],
)