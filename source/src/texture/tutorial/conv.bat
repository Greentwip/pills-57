@echo off

cd %1\alpha
..\..\%DM64ROOT%\tool\bmp2tex *.bmp

cd ..\color
..\..\%DM64ROOT%\tool\bmp2tex -TC 255,0,255 -FC *.bmp

cd ..\normal
..\..\%DM64ROOT%\tool\bmp2tex -TC 255,0,255 *.bmp

cd ..\normal2
..\..\%DM64ROOT%\tool\bmp2tex -TI 0 *.bmp

cd ..\..

