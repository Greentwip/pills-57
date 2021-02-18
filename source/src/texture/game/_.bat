@echo off

set DM64ROOT=..\..
if "%DM64ROOT%"=="" goto error



call conv al
call conv p1
call conv p2
call conv p4
call conv ls

cd item
..\%DM64ROOT%\tool\bmp2tex -TI 0 *.bmp
cd ..



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
