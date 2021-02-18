@echo off

cd %1\ani
..\..\%DM64ROOT%\tool\bmp2tex -T00 *.bmp

rem cd ..\int
rem ..\..\%DM64ROOT%\tool\bmp2tex *.bmp

cd ..\..

