@echo off

set I10ROOT=..
if "%I10ROOT%"=="" goto error

cd ..\assets\zh\tutorial

cd all\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp

cd ..\..\..\..\..\tools

goto end

:error
pause

:end
