@echo off

md %OUTDIR%
md %OUTDIR%\%LOCAL%
del *.o
copy %OUTDIR%\%LOCAL%\*.o

del version.c
tool\ptee -e err.txt make OUTDIR=%OUTDIR% LOCAL=%LOCAL%
del *.o
