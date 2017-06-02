@echo off
if "%1" == "clean" goto CLEAN
if "%1" == "CLEAN" goto CLEAN

if not exist lib md lib
if not exist lib\vc32 md lib\vc32
if not exist obj md obj
if not exist obj\vc32 md obj\vc32
if not exist obj\vc32\uni md obj\vc32\uni

@set PATH=C:\Softools\MSVS10\VC\bin;C:\Softools\MSVS10\Common7\IDE;%PATH%
@set INCLUDE=C:\Softools\MSVS10\VC\include;C:\Softools\mssdk7\include
@set LIB=C:\Softools\MSVS10\VC\lib
@set LIBPATH=C:\Softools\MSVS10\VC\lib

nmake /I /Fmakefile.vc %1 %2 %3 >make_vc.log 2>make_vc_err.log
if errorlevel 1 goto BUILD_ERR

set CFLAGS=-DUNICODE
set UNICODE=Yes
nmake /I /Fmakefile.vc %1 %2 %3 >>make_vc.log 2>>make_vc_err.log

   goto EXIT

:BUILD_ERR

   notepad make_vc_err.log
   goto EXIT

:CLEAN
   del lib\*.lib
   del lib\*.bak
   del obj\vc32\*.obj
   del obj\vc32\uni\*.obj

   del make_vc.log
   del make_vc_err.log

   goto EXIT

:EXIT

