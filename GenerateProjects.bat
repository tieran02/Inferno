@echo off
echo "Calling bootstrap to get all dependencies"
call bootstrap.py

echo "Calling sharpmake to generate project"
call sharpmake\bin\Sharpmake.Application.exe /sources('sharpmake/main.sharpmake.cs')
pause