# OpenGL

## dependencies package

- nixos

```nix
buildInputs = with pkgs; [
    glew
    libGL
    glfw
];
shellHook = ''
    export LD_LIBRARY_PATH="${pkgs.libGL}/lib:${pkgs.glew}/lib:$LD_LIBRARY_PATH"
'';
```

## header

opengl have to import this header

```c++
#include <GL/glew.h>
#include <GLFW/glfw3.h>
```

- `glew.h` library for set API from any thardware to standart software API, for make glfw3 library can communicate with GPU.
- `glfw3.h` library is a standart library for opengl rendering and screen window creating.

## how OpenGL work

```mermaid
---
title: OpenGL process
---
flowchart LR
start(["start"])
stop(["stop"])

setup_window["
Setup Window
- initGL
- hint window
- create instance window
- make context
- setup error massage
"]

setup_GL["
Setup OpenGL
- compile vertex
- compile fragment
- compile shaderprogram
- loading object position
- set how to read object
"]

loop_render["
Loop rendering
- clear background screen
- process input
- create matrix rotation from input
- complie matrix and object
- reandering
"]

cleanup["
Clean up
- delete shader program
- delete object vertex
- delete window instance
- GL terminate
"]

start --> setup_window
setup_window --> setup_GL
setup_GL --> loop_render
loop_render --> cleanup
cleanup --> stop
```

### learning resource

- [VAO](https://youtu.be/hG5p7OSP3Wk?si=vPqpjoNxMZy3iShq)
- [vertex and fragment shader](https://youtu.be/KqwvGAkKTtU?si=cCW4Sbo8YWR6guLo)

#### context window

Basically gpu will use thread for rendering, function `glfwMakeContextCurrent(_window);`
just send to openGL to lock flags, that use thread for create image to this window only.

#### glew

When you want to use modern feature such as shader, texture, vertex buffer you have to query OS and GPU driver
to manually load pointer to these function in memory, GLEW will load and handle that automatically.

---

### Runtime rendering

```mermaid
---
title: OpenGL Rendering process
---
stateDiagram-v2

state "initialize Window, Opengl and VAO" as init
state "check window should close" as startloop
state "clear and setup beckground" as setBackground
state "process Input matching" as getInput
state "setup rotate translate and zoom matrix" as setMatrix
state "process vertex shader" as vertex
state "process fragment shader" as fragment
state "put object into window" as render
state "cleanup object window and GLinstance" as cleanup

[*] --> init
init --> startloop : startloop
startloop --> setBackground
setBackground --> getInput : get input from user
getInput --> setMatrix
setMatrix --> vertex : pass VAO and matrix to vertext shader
vertex --> fragment
fragment --> render
render --> startloop : finish render loop

startloop --> cleanup : if client close window
cleanup --> [*]

```
