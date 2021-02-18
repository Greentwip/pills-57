@echo off

set I10ROOT=..
if "%I10ROOT%"=="" goto error

cd ..\assets\zh\game

cd al\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd p1\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd p2\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\..

cd p4\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd ls\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd item
..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..\..\..\tools

goto end

:error
pause

:end
