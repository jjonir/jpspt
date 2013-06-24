jpspt
=====

A shader prototyping tool. Written primarily for practice, but it might also
be useful someday.

Using
-----
Loads some vertex shader and fragment shader by default.  
These keys prompt you to enter the name of a file:  
f - load a fragment shader for the main geometry.  
o - load a fragment shader for the outline.  
v - load a vertex shader for the main geometry.  
V - load a vertex shader for both the main geometry and the outline.  
g - load a .obj file and switch geometry to that model, if it loads.
The .obj loader currently supports a very small subset of the full .obj format.

F5 - reload all shaders and models from their files.  
F11 - switch to fullscreen.  
ESC - quit if displaying a shader, or cancel if entering a filename.  
TAB - switch geometry between a bunch of different models to shade.

w - move closer.  
s - move farther.

Mouse controls:  
Leftclick and drag to rotate the geometry.  
Rightclick and drag to move the geometry.

Dependencies
------------

Requires glut and glew for now.

