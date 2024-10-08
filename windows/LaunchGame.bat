@echo off
set cwd=%cd%
set old_path=%PATH%

PATH=%cwd%\dlls;%PATH%
start ./bin/game.exe

PATH=%old_path%