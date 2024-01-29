@echo off
echo "Calling bootstrap to get all dependencies"
call bootstrap.py

echo "Calling sharpmake to generate project"
call sharpmake\bin\Sharpmake.Application.exe /sources('sharpmake/engineDefs.sharpmake.cs', 'sharpmake/main.sharpmake.cs', 'sharpmake/editor.sharpmake.cs', 'sharpmake/core.sharpmake.cs', 'sharpmake/graphics.sharpmake.cs', 'sharpmake/thirdparty/glfw.sharpmake.cs')
pause