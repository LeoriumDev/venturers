# Venturers

A Minecraft-like voxel game built with C++ and OpenGL.

## Features

- Basic Camera Movement
- Simple procedural terrain generation

## Platforms

- macOS / Linux / Windows

I tested this game primarily on macOS, but other platforms should work in theory.

## Build

### Requirements

- C++23 or newer
- CMake ≥ 4.0
- OpenGL-capable graphics driver (most computers do)

### Clone the repository

```bash
git clone --recursive https://github.com/LeoriumDev/venturers.git
cd venturers
```

### Build with CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Run

```bash
./venturers
```

(On Windows, the executable will be located in the build output directory.)

## Controls

- Press W / A / S / D to move around
- Press Space to go up, and Left Shift to go down
- Use the mouse to look around
- Press ESC to exit the game

## Credits

Huge thanks to these websites:
- [Learn OpenGL](https://learnopengl.com)
- [Using Perlin Noise to Generate 2D Terrain and Water](https://gpfault.net/posts/perlin-noise.txt.html)
