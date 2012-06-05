@echo off

call "%VS100COMNTOOLS%\vsvars32.bat"

cl.exe /DWINDOWS /c utilities.c

cl.exe /DWINDOWS add_colour.c utilities.obj
cl.exe /DWINDOWS add_normals.c utilities.obj

cl.exe /DWINDOWS strip_colour.c utilities.obj
cl.exe /DWINDOWS strip_normals.c utilities.obj

cl.exe /DWINDOWS normalise_position.c utilities.obj
cl.exe /DWINDOWS scale_mesh.c utilities.obj
cl.exe /DWINDOWS swap_axes.c utilities.obj
cl.exe /DWINDOWS mesh_size.c utilities.obj

pause
