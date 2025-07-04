#include <raylib.h>
#include <gruvbox.h>
#include <math.h>

#include "raygui.h"
#include "constants.h"

// Function to draw color palette picker
int Palette(const char* title) {

    // Semi-transparent overlay
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 128});

    // Palette window
    int paletteWidth = 400;
    int paletteHeight = 300;
    int paletteX = (WINDOW_WIDTH - paletteWidth) / 2;
    int paletteY = (WINDOW_HEIGHT - paletteHeight) / 2;

    DrawRectangle(paletteX, paletteY, paletteWidth, paletteHeight, Gruvbox[GRUVBOX_DARK1]);
    DrawRectangleLines(paletteX, paletteY, paletteWidth, paletteHeight, Gruvbox[GRUVBOX_LIGHT1]);

    // Title
    DrawTextEx(GetFontDefault(), title, (Vector2){paletteX + 10, paletteY + 10}, 20, 1, Gruvbox[GRUVBOX_LIGHT1]);

    // Color grid
    int grid_size = ceil(sqrt(GRUVBOX_NUM_COLORS));
    int colorWidth = (paletteWidth - 40) / grid_size;
    int colorHeight = (paletteHeight - 80) / grid_size;
    int currentColor = 0;

    for(int i = 0; i < grid_size; i++){
        for(int j = 0; j < grid_size; j++){
            if(currentColor < GRUVBOX_NUM_COLORS){
                Rectangle colorRect = {
                    paletteX + 20 + i * colorWidth,
                    paletteY + 40 + j * colorHeight,
                    colorWidth - 2,
                    colorHeight - 2
                };

                DrawRectangleRec(colorRect, Gruvbox[currentColor]);
                DrawRectangleLinesEx(colorRect, 1, Gruvbox[GRUVBOX_LIGHT1]);

                // Check for click
                if (CheckCollisionPointRec(GetMousePosition(), colorRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    return currentColor;
                }

                currentColor++;
            }
        }
    }

    // Close button
    Rectangle closeRect = {paletteX + paletteWidth - 60, paletteY + 10, 50, 25};
    if (GuiButton(closeRect, "Close")) {
        return -2;
    }

    return -1;
}
