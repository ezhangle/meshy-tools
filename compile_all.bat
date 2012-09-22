@echo off

call "%VS110COMNTOOLS%\vsvars32.bat"

msbuild /nologo /verbosity:minimal /property:Configuration=Release visual_studio\offtools.sln

pause
