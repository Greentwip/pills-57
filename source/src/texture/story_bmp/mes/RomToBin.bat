@echo off
rem このﾌｧｲﾙは毎回更新されます

ren codesegment.o codesegment.bak
del *.o
ren codesegment.bak codesegment.o
del rom.n64
del rom.out

make
if errorlevel 1 goto error
mild -r rom.n64 -e rom.out spec
if errorlevel 1 goto error
nm rom.out | "D:\Data\DownLoad\小林のツール\RomToBin\nmsym" rom.n64
if errorlevel 1 goto error
goto end

:error
pause

:end
ren codesegment.o codesegment.bak
del *.o
ren codesegment.bak codesegment.o
del rom.n64
del rom.out
