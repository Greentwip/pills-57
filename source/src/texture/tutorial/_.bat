@echo off

set DM64ROOT=..\..
if "%DM64ROOT%"=="" goto error


call conv all


mkdir JAPAN
make clean
make LOCAL=JAPAN
if errorlevel 1 goto error

mkdir AMERICA
make clean
make LOCAL=AMERICA
if errorlevel 1 goto error

make clean
goto end

:error
pause

:end
