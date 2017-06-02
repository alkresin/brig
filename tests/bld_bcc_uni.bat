@echo off
set BRIG_DIR=\papps\brig
set BRIG_LIBS=brig.lib

bcc32 -c -O2 -tW -M -I%BRIG_DIR%\include -DUNICODE -D_UNICODE %1.cpp >err.out

echo 1 24 "..\include\WindowsXP.Manifest" > hwgui_xp.rc
brc32 -r hwgui_xp -fohwgui_xp

ilink32 -Gn -aa -Tpe -L%BRIG_DIR%\lib\b32 c0w32.obj %1.obj, %1.exe, %1.map, %BRIG_LIBS% ws2_32.lib cw32.lib import32.lib,, hwgui_xp.res

del *.obj
del *.map
del *.tds
del *.res