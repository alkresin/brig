@echo off
if "%1" == "clean" goto CLEAN
if "%1" == "CLEAN" goto CLEAN

:BUILD

if not exist lib md lib
if not exist lib\w32 md lib\w32
if not exist obj md obj
if not exist obj\w32 md obj\w32
if not exist obj\w32\uni md obj\w32\uni

   set path=c:\softools\qt\tools\mingw482_32\bin
   mingw32-make.exe -f makefile.gcc >make_mingw.log 2>make_mingw_err.log

   if errorlevel 1 goto BUILD_ERR

   set CFLAGS=-DUNICODE -D_UNICODE
   set UNICODE=yes
   mingw32-make.exe -f makefile.gcc >>make_mingw.log 2>>make_mingw_err.log

   if errorlevel 1 goto BUILD_ERR

:BUILD_OK

   goto EXIT

:BUILD_ERR

   goto EXIT

:CLEAN
   del lib\*.a
   del obj\w32\*.o
   del obj\w32\uni\*.o
   del make_mingw.log
   del make_mingw_err.log

   goto EXIT

:EXIT
