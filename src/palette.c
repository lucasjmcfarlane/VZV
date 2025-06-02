#include <raylib.h>
#include <gruvbox.h>
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define APPLICATION_NAME "Palette"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000
#define FONT_SIZE 18
#define LINE_SPACING 5
#define TARGET_FPS 1

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
    SetTargetFPS(TARGET_FPS);

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        int grid_size = ceil(sqrt(NUM_COLORS));
        int current_color = 0;

        for(int i = 0; i<grid_size; i++){
            for(int j = 0; j<grid_size; j++){
                if(current_color<NUM_COLORS){
                    DrawRectangle(i*(WINDOW_WIDTH/grid_size),
                            j*(WINDOW_HEIGHT/grid_size),
                            WINDOW_WIDTH/grid_size,
                            WINDOW_HEIGHT/grid_size,
                            Gruvbox[current_color]);

                    DrawText(GetColorName(current_color), i*(WINDOW_WIDTH/grid_size), j*(WINDOW_HEIGHT/grid_size), FONT_SIZE,
                            (Color){255-Gruvbox[current_color].r, 255-Gruvbox[current_color].g ,255-Gruvbox[current_color].b, 255});

                    current_color++;
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
