@echo off

set DM64ROOT=..\..
if "%DM64ROOT%"=="" goto error


call conv char
call conv common
call conv cont
call conv level
call conv main
call conv name
call conv p2
call conv p4
call conv rank
call conv setup
call conv story
call conv kasa


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
