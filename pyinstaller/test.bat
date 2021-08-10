pushd dist\reversi

set PATH_SAVE=%PATH%
set PATH=C:\WINDOWS\System32;

reversi.exe

set PATH=%PATH_SAVE%

popd