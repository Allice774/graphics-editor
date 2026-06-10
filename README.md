# 2D Graphics Editor in C

A menu-driven, lightweight, and robust console-based 2D Graphics Editor written in standard C. This editor uses a 2D character array as a canvas where empty pixels are represented by `_` and drawn shape pixels are represented by `*`.

## Features
- **Visual Grid Canvas**: A 40x20 text-based canvas with column and row coordinates printed around it.
- **Dynamic Render Registry**: Shapes are managed in an in-memory registry, allowing shapes to be added, modified, or deleted interactively. Whenever a change occurs, the canvas is automatically cleared and redrawn.
- **Robust Input Handling**: Protects against invalid inputs, characters inside numeric fields, and out-of-bounds inputs using safe standard input buffers (`fgets` + `strtol`).
- **Precision Drawing Algorithms**:
  - **Line**: Bresenham's Line Algorithm.
  - **Circle**: Midpoint Circle Algorithm (Bresenham's Circle).
  - **Rectangle**: Computes outline boundaries and plots them on the canvas grid.
  - **Triangle**: Renders three intersecting lines connecting the vertices.

## Getting Started

### Prerequisites
You will need a standard C compiler (such as GCC, Clang, or MSVC) installed on your system.

### Compilation

#### Using GCC / MinGW:
```bash
gcc -Wall -Wextra -std=c99 -o graphics_editor.exe graphics_editor.c
```

#### Using MSVC (Visual Studio Developer Command Prompt):
```cmd
cl /Fe:graphics_editor.exe graphics_editor.c
```

### Running the Editor
After compilation, run the executable:
```bash
./graphics_editor.exe
```

## How to Use
1. **Add a Shape**: Choose option `1` from the main menu, select the shape type, and enter the parameters (e.g. coordinates or radius).
2. **Modify a Shape**: Choose option `2`, specify the ID of the shape you wish to change, and enter its new coordinates.
3. **Delete a Shape**: Choose option `3` and enter the shape ID to remove it from the canvas.
4. **Clear Canvas**: Choose option `4` to erase all shapes.
5. **Exit**: Choose option `5` to close the program.
