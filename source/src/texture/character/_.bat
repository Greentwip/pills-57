@echo off

set DM64ROOT=..\..
if "%DM64ROOT%"=="" goto error


call conv a
call conv a2
call conv b
call conv c
call conv d
call conv e
call conv f
call conv g
call conv h
call conv i
call conv j
call conv k
call conv l
call conv m
call conv n
call conv o
call conv mario
call conv virus_b
call conv virus_r
call conv virus_y
call conv smog


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
