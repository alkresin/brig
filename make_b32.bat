@echo off
if "%1" == "clean" goto CLEAN
if "%1" == "CLEAN" goto CLEAN

if not exist lib md lib
if not exist obj md obj
if not exist obj\b32 md obj\b32
if not exist obj\b32\uni md obj\b32\uni
:BUILD

make -l -fmakefile.bc %1 %2 %3 > make_b32.log

if errorlevel 1 goto BUILD_ERR

set CFLAGS=-DUNICODE
make -l -DUNICODE -fmakefile.bc %1 %2 %3 >> make_b32.log

if errorlevel 1 goto BUILD_ERR

:BUILD_OK

   goto EXIT

:BUILD_ERR

   notepad make_b32.log
   goto EXIT

:CLEAN
   del lib\*.lib
   del lib\*.bak
   del obj\b32\*.obj
   del obj\b32\uni\*.obj

   del make_b32.log

   goto EXIT

:EXIT

