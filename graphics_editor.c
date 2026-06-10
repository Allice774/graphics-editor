#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_SHAPES 100

// Define shape types
typedef enum {
    LINE,
    CIRCLE,
    RECTANGLE,
    TRIANGLE
} ShapeType;

// Define parameters for shapes
typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;

typedef struct {
    int xc, yc;
    int r;
} CircleParams;

typedef struct {
    int x1, y1; // Top-left corner
    int x2, y2; // Bottom-right corner
} RectParams;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriParams;

// Representing a shape
typedef struct {
    int id;
    ShapeType type;
    union {
        LineParams line;
        CircleParams circle;
        RectParams rect;
        TriParams tri;
    } params;
    int isActive;
} Shape;

// Global Registry of shapes
Shape shapes[MAX_SHAPES];
int nextShapeId = 1;

// Function declarations
void clearCanvas(char canvas[HEIGHT][WIDTH]);
void plotPixel(char canvas[HEIGHT][WIDTH], int x, int y);
void drawLine(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2);
void drawCircle(char canvas[HEIGHT][WIDTH], int xc, int yc, int r);
void drawRectangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2);
void drawTriangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3);
void displayCanvas(char canvas[HEIGHT][WIDTH]);
void redrawAll(char canvas[HEIGHT][WIDTH]);
void addShapeMenu();
void modifyShapeMenu();
void deleteShapeMenu();
void printShapesList();
int getIntegerInput(const char* prompt, int minVal, int maxVal);
void printBanner();

// Cross-platform Clear Screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[J");
#endif
}

int main() {
    // Initialize standard streams buffer behavior to ensure clean displays
    setvbuf(stdout, NULL, _IONBF, 0);

    char canvas[HEIGHT][WIDTH];
    int choice;

    // Initialize all shapes registry
    for (int i = 0; i < MAX_SHAPES; i++) {
        shapes[i].isActive = 0;
    }

    while (1) {
        clearScreen();
        printBanner();
        printShapesList();
        
        // Redraw canvas and render it
        redrawAll(canvas);
        displayCanvas(canvas);

        printf("\n=== Menu ===\n");
        printf("1. Add a Shape\n");
        printf("2. Modify a Shape\n");
        printf("3. Delete a Shape\n");
        printf("4. Clear Canvas (Delete All)\n");
        printf("5. Exit\n");

        choice = getIntegerInput("Enter choice (1-5): ", 1, 5);

        if (choice == 5) {
            printf("\nExiting. Thank you for using the 2D Graphics Editor!\n");
            break;
        }

        switch (choice) {
            case 1:
                addShapeMenu();
                break;
            case 2:
                modifyShapeMenu();
                break;
            case 3:
                deleteShapeMenu();
                break;
            case 4:
                for (int i = 0; i < MAX_SHAPES; i++) {
                    shapes[i].isActive = 0;
                }
                nextShapeId = 1;
                printf("\nCanvas cleared successfully! Press Enter to continue...");
                getchar();
                break;
            default:
                break;
        }
    }

    return 0;
}

// Clear the canvas to underscores
void clearCanvas(char canvas[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Plot a pixel safely within boundary
void plotPixel(char canvas[HEIGHT][WIDTH], int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
void drawLine(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        plotPixel(canvas, x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Plot 8 symmetrical circle points
void plotCirclePoints(char canvas[HEIGHT][WIDTH], int xc, int yc, int x, int y) {
    plotPixel(canvas, xc + x, yc + y);
    plotPixel(canvas, xc - x, yc + y);
    plotPixel(canvas, xc + x, yc - y);
    plotPixel(canvas, xc - x, yc - y);
    plotPixel(canvas, xc + y, yc + x);
    plotPixel(canvas, xc - y, yc + x);
    plotPixel(canvas, xc + y, yc - x);
    plotPixel(canvas, xc - y, yc - x);
}

// Midpoint Circle Algorithm
void drawCircle(char canvas[HEIGHT][WIDTH], int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    plotCirclePoints(canvas, xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plotCirclePoints(canvas, xc, yc, x, y);
    }
}

// Rectangle drawing by drawing horizontal and vertical line segments
void drawRectangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int minX = x1 < x2 ? x1 : x2;
    int maxX = x1 > x2 ? x1 : x2;
    int minY = y1 < y2 ? y1 : y2;
    int maxY = y1 > y2 ? y1 : y2;

    // Draw horizontal lines (top and bottom)
    for (int x = minX; x <= maxX; x++) {
        plotPixel(canvas, x, minY);
        plotPixel(canvas, x, maxY);
    }

    // Draw vertical lines (left and right)
    for (int y = minY; y <= maxY; y++) {
        plotPixel(canvas, minX, y);
        plotPixel(canvas, maxX, y);
    }
}

// Triangle drawing using three line segments
void drawTriangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(canvas, x1, y1, x2, y2);
    drawLine(canvas, x2, y2, x3, y3);
    drawLine(canvas, x3, y3, x1, y1);
}

// Render canvas to console with proper formatting
void displayCanvas(char canvas[HEIGHT][WIDTH]) {
    // Print column guide markers at the top
    printf("   ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 5 == 0) {
            printf("%-2d", x);
        } else if (x % 5 != 1) {
            printf("  ");
        }
    }
    printf("\n");

    // Print top boundary line
    printf("  +");
    for (int x = 0; x < WIDTH; x++) {
        printf("--");
    }
    printf("-+\n");

    // Print rows
    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d| ", y); // row label
        for (int x = 0; x < WIDTH; x++) {
            printf("%c ", canvas[y][x]);
        }
        printf("|\n");
    }

    // Print bottom boundary line
    printf("  +");
    for (int x = 0; x < WIDTH; x++) {
        printf("--");
    }
    printf("-+\n");
}

// Redraw canvas from scratch based on active shapes
void redrawAll(char canvas[HEIGHT][WIDTH]) {
    clearCanvas(canvas);
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].isActive) {
            switch (shapes[i].type) {
                case LINE:
                    drawLine(canvas, shapes[i].params.line.x1, shapes[i].params.line.y1,
                             shapes[i].params.line.x2, shapes[i].params.line.y2);
                    break;
                case CIRCLE:
                    drawCircle(canvas, shapes[i].params.circle.xc, shapes[i].params.circle.yc,
                               shapes[i].params.circle.r);
                    break;
                case RECTANGLE:
                    drawRectangle(canvas, shapes[i].params.rect.x1, shapes[i].params.rect.y1,
                                  shapes[i].params.rect.x2, shapes[i].params.rect.y2);
                    break;
                case TRIANGLE:
                    drawTriangle(canvas, shapes[i].params.tri.x1, shapes[i].params.tri.y1,
                                 shapes[i].params.tri.x2, shapes[i].params.tri.y2,
                                 shapes[i].params.tri.x3, shapes[i].params.tri.y3);
                    break;
            }
        }
    }
}

// List all active shapes in a table
void printShapesList() {
    printf("Active Shapes:\n");
    printf("-------------------------------------------------------------\n");
    printf("| %-4s | %-10s | %-37s |\n", "ID", "Type", "Coordinates / Parameters");
    printf("-------------------------------------------------------------\n");
    int count = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].isActive) {
            count++;
            char details[100];
            switch (shapes[i].type) {
                case LINE:
                    sprintf(details, "Start: (%d, %d), End: (%d, %d)",
                            shapes[i].params.line.x1, shapes[i].params.line.y1,
                            shapes[i].params.line.x2, shapes[i].params.line.y2);
                    printf("| %-4d | %-10s | %-37s |\n", shapes[i].id, "Line", details);
                    break;
                case CIRCLE:
                    sprintf(details, "Center: (%d, %d), Radius: %d",
                            shapes[i].params.circle.xc, shapes[i].params.circle.yc,
                            shapes[i].params.circle.r);
                    printf("| %-4d | %-10s | %-37s |\n", shapes[i].id, "Circle", details);
                    break;
                case RECTANGLE:
                    sprintf(details, "Corner 1: (%d, %d), Corner 2: (%d, %d)",
                            shapes[i].params.rect.x1, shapes[i].params.rect.y1,
                            shapes[i].params.rect.x2, shapes[i].params.rect.y2);
                    printf("| %-4d | %-10s | %-37s |\n", shapes[i].id, "Rectangle", details);
                    break;
                case TRIANGLE:
                    sprintf(details, "P1: (%d,%d), P2: (%d,%d), P3: (%d,%d)",
                            shapes[i].params.tri.x1, shapes[i].params.tri.y1,
                            shapes[i].params.tri.x2, shapes[i].params.tri.y2,
                            shapes[i].params.tri.x3, shapes[i].params.tri.y3);
                    printf("| %-4d | %-10s | %-37s |\n", shapes[i].id, "Triangle", details);
                    break;
            }
        }
    }
    if (count == 0) {
        printf("| %-57s |\n", "No shapes added yet.");
    }
    printf("-------------------------------------------------------------\n\n");
}

// Menu interface to add a shape
void addShapeMenu() {
    int slot = -1;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].isActive) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        printf("Error: Maximum shape limit (%d) reached. Delete some shapes first.\n", MAX_SHAPES);
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    clearScreen();
    printBanner();
    printf("=== Add a New Shape ===\n");
    printf("1. Line\n");
    printf("2. Circle\n");
    printf("3. Rectangle\n");
    printf("4. Triangle\n");
    printf("5. Back to Main Menu\n");
    
    int typeChoice = getIntegerInput("Select shape type (1-5): ", 1, 5);
    if (typeChoice == 5) return;

    shapes[slot].id = nextShapeId++;
    shapes[slot].isActive = 1;

    switch (typeChoice) {
        case 1: // LINE
            shapes[slot].type = LINE;
            shapes[slot].params.line.x1 = getIntegerInput("Enter x1 (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.line.y1 = getIntegerInput("Enter y1 (0-19): ", 0, HEIGHT - 1);
            shapes[slot].params.line.x2 = getIntegerInput("Enter x2 (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.line.y2 = getIntegerInput("Enter y2 (0-19): ", 0, HEIGHT - 1);
            break;
        case 2: // CIRCLE
            shapes[slot].type = CIRCLE;
            shapes[slot].params.circle.xc = getIntegerInput("Enter center x (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.circle.yc = getIntegerInput("Enter center y (0-19): ", 0, HEIGHT - 1);
            shapes[slot].params.circle.r = getIntegerInput("Enter radius r (1-20): ", 1, 20);
            break;
        case 3: // RECTANGLE
            shapes[slot].type = RECTANGLE;
            shapes[slot].params.rect.x1 = getIntegerInput("Enter corner 1 x (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.rect.y1 = getIntegerInput("Enter corner 1 y (0-19): ", 0, HEIGHT - 1);
            shapes[slot].params.rect.x2 = getIntegerInput("Enter corner 2 x (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.rect.y2 = getIntegerInput("Enter corner 2 y (0-19): ", 0, HEIGHT - 1);
            break;
        case 4: // TRIANGLE
            shapes[slot].type = TRIANGLE;
            shapes[slot].params.tri.x1 = getIntegerInput("Enter point 1 x (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.tri.y1 = getIntegerInput("Enter point 1 y (0-19): ", 0, HEIGHT - 1);
            shapes[slot].params.tri.x2 = getIntegerInput("Enter point 2 x (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.tri.y2 = getIntegerInput("Enter point 2 y (0-19): ", 0, HEIGHT - 1);
            shapes[slot].params.tri.x3 = getIntegerInput("Enter point 3 x (0-39): ", 0, WIDTH - 1);
            shapes[slot].params.tri.y3 = getIntegerInput("Enter point 3 y (0-19): ", 0, HEIGHT - 1);
            break;
    }

    printf("\nShape added successfully! Press Enter to return to main menu...");
    getchar();
}

// Menu interface to modify a shape
void modifyShapeMenu() {
    int activeCount = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].isActive) activeCount++;
    }

    if (activeCount == 0) {
        printf("\nNo shapes available to modify. Press Enter to continue...");
        getchar();
        return;
    }

    int targetId = getIntegerInput("Enter the ID of the shape to modify: ", 1, nextShapeId - 1);
    
    int index = -1;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].isActive && shapes[i].id == targetId) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Error: Shape ID %d not found.\n", targetId);
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    printf("\nModifying shape ID %d (%s):\n", targetId, 
           shapes[index].type == LINE ? "Line" :
           shapes[index].type == CIRCLE ? "Circle" :
           shapes[index].type == RECTANGLE ? "Rectangle" : "Triangle");

    switch (shapes[index].type) {
        case LINE:
            printf("Current: Start (%d, %d), End (%d, %d)\n", 
                   shapes[index].params.line.x1, shapes[index].params.line.y1,
                   shapes[index].params.line.x2, shapes[index].params.line.y2);
            shapes[index].params.line.x1 = getIntegerInput("Enter new x1 (0-39): ", 0, WIDTH - 1);
            shapes[index].params.line.y1 = getIntegerInput("Enter new y1 (0-19): ", 0, HEIGHT - 1);
            shapes[index].params.line.x2 = getIntegerInput("Enter new x2 (0-39): ", 0, WIDTH - 1);
            shapes[index].params.line.y2 = getIntegerInput("Enter new y2 (0-19): ", 0, HEIGHT - 1);
            break;
        case CIRCLE:
            printf("Current: Center (%d, %d), Radius %d\n", 
                   shapes[index].params.circle.xc, shapes[index].params.circle.yc,
                   shapes[index].params.circle.r);
            shapes[index].params.circle.xc = getIntegerInput("Enter new center x (0-39): ", 0, WIDTH - 1);
            shapes[index].params.circle.yc = getIntegerInput("Enter new center y (0-19): ", 0, HEIGHT - 1);
            shapes[index].params.circle.r = getIntegerInput("Enter new radius r (1-20): ", 1, 20);
            break;
        case RECTANGLE:
            printf("Current: Corner 1 (%d, %d), Corner 2 (%d, %d)\n", 
                   shapes[index].params.rect.x1, shapes[index].params.rect.y1,
                   shapes[index].params.rect.x2, shapes[index].params.rect.y2);
            shapes[index].params.rect.x1 = getIntegerInput("Enter new corner 1 x (0-39): ", 0, WIDTH - 1);
            shapes[index].params.rect.y1 = getIntegerInput("Enter new corner 1 y (0-19): ", 0, HEIGHT - 1);
            shapes[index].params.rect.x2 = getIntegerInput("Enter new corner 2 x (0-39): ", 0, WIDTH - 1);
            shapes[index].params.rect.y2 = getIntegerInput("Enter new corner 2 y (0-19): ", 0, HEIGHT - 1);
            break;
        case TRIANGLE:
            printf("Current: P1 (%d, %d), P2 (%d, %d), P3 (%d, %d)\n", 
                   shapes[index].params.tri.x1, shapes[index].params.tri.y1,
                   shapes[index].params.tri.x2, shapes[index].params.tri.y2,
                   shapes[index].params.tri.x3, shapes[index].params.tri.y3);
            shapes[index].params.tri.x1 = getIntegerInput("Enter new point 1 x (0-39): ", 0, WIDTH - 1);
            shapes[index].params.tri.y1 = getIntegerInput("Enter new point 1 y (0-19): ", 0, HEIGHT - 1);
            shapes[index].params.tri.x2 = getIntegerInput("Enter new point 2 x (0-39): ", 0, WIDTH - 1);
            shapes[index].params.tri.y2 = getIntegerInput("Enter new point 2 y (0-19): ", 0, HEIGHT - 1);
            shapes[index].params.tri.x3 = getIntegerInput("Enter new point 3 x (0-39): ", 0, WIDTH - 1);
            shapes[index].params.tri.y3 = getIntegerInput("Enter new point 3 y (0-19): ", 0, HEIGHT - 1);
            break;
    }

    printf("\nShape modified successfully! Press Enter to return to main menu...");
    getchar();
}

// Menu interface to delete a shape
void deleteShapeMenu() {
    int activeCount = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].isActive) activeCount++;
    }

    if (activeCount == 0) {
        printf("\nNo shapes available to delete. Press Enter to continue...");
        getchar();
        return;
    }

    int targetId = getIntegerInput("Enter the ID of the shape to delete: ", 1, nextShapeId - 1);

    int index = -1;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].isActive && shapes[i].id == targetId) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Error: Shape ID %d not found.\n", targetId);
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    shapes[index].isActive = 0;
    printf("\nShape ID %d deleted successfully! Press Enter to return to main menu...", targetId);
    getchar();
}

// Robust input reader function
int getIntegerInput(const char* prompt, int minVal, int maxVal) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        
        char* endptr;
        // Base 10 parsing
        long temp = strtol(buffer, &endptr, 10);
        
        // Check for parsing error
        if (endptr == buffer || (*endptr != '\n' && *endptr != '\0')) {
            printf("Error: Please enter a valid integer.\n");
            continue;
        }
        
        // Bounds checking
        if (temp < minVal || temp > maxVal) {
            printf("Error: Input out of range (%d to %d).\n", minVal, maxVal);
            continue;
        }
        
        val = (int)temp;
        break;
    }
    return val;
}

// Beautiful Banner
void printBanner() {
    printf("*************************************************************\n");
    printf("*                  2D GRAPHICS EDITOR IN C                  *\n");
    printf("*************************************************************\n");
}
