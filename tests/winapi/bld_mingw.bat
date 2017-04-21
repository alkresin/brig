@echo off
set path=c:\softools\qt\tools\mingw482_32\bin
set BRIG_DIR=\papps\brig
set BRIG_LIBS=-lbrig_ansi

g++ -I. -I%BRIG_DIR%\include -Wall -c %1.cpp -o%1.o
g++ -Wall -mwindows -static -o%1.exe %1.o -L%BRIG_DIR%\lib -Wl,--allow-multiple-definition -Wl,--start-group %BRIG_LIBS% -lmingw32 -luser32 -lwinspool -lcomctl32 -lcomdlg32 -lgdi32 -lole32 -loleaut32 -luuid -lwinmm -Wl,--end-group

del *.o
