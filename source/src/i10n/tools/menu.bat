@echo off

set I10ROOT=..
if "%I10ROOT%"=="" goto error

cd ..\assets\zh\menu

cd char\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd common\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd level\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd cont\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd main\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd name\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd p2\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd p4\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd rank\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd setup\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd story\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\normal
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 *.bmp
cd ..\normal2
..\..\..\..\%I10ROOT%\tools\bmp2tex -TI 0 *.bmp
cd ..\..

cd kasa\alpha
..\..\..\..\%I10ROOT%\tools\bmp2tex *.bmp
cd ..\color
..\..\..\..\%I10ROOT%\tools\bmp2tex -TC 255,0,255 -FC *.bmp
cd ..\..\..\..\..\tools

goto end

:error
pause

:end
