@echo off

call "%VS100COMNTOOLS%\vsvars32.bat"

cl.exe /c utilities.c

cl.exe add_colour.c utilities.obj
cl.exe add_normals.c utilities.obj

cl.exe strip_colour.c utilities.obj
cl.exe strip_normals.c utilities.obj

cl.exe normalise_position.c utilities.obj
cl.exe scale_mesh.c utilities.obj
cl.exe swap_axes.c utilities.obj
cl.exe mesh_size.c utilities.obj

pause








