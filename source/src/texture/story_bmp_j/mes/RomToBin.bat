@echo off
rem ����̧�ق͖���X�V����܂�

ren codesegment.o codesegment.bak
del *.o
ren codesegment.bak codesegment.o
del rom.n64
del rom.out

make
if errorlevel 1 goto error
mild -r rom.n64 -e rom.out spec
if errorlevel 1 goto error
nm rom.out | "D:\Data\DownLoad\���т̃c�[��\RomToBin\nmsym" rom.n64
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
