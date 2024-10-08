@echo off
set cwd=%cd%
set old_path=%PATH%

PATH=%cwd%\dlls;%PATH%
start ./bin/engine.exe

PATH=%old_path%