# Another Sidescroller
This is a small prototype for a sidescrolling game.

![Screenshot](screen.gif)

### Features
* Animated character moving left and right
* Climbing ladders up and down
* Mouse controls
* The map is created from text files
* Multiplayer support for 2 players via TCP

## Installing

Building the game requires some libraries: SDL2, SDL2\_ttf, SDL2\_net, SDL2\_image, OpenGL and GLEW. For SDL see http://www.libsdl.org/.

A very simple makefile is included. You can type
```
make
```
to build the project with g++.