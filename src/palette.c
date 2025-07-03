#include <raylib.h>
#include <gruvbox.h>
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION // Avoid including raygui implementation again

#include "constants.h"

#define APPLICATION_NAME "Palette"

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
    SetTargetFPS(TARGET_FPS);

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        int grid_size = ceil(sqrt(GRUVBOX_NUM_COLORS));
        int current_color = 0;

        for(int i = 0; i<grid_size; i++){
            for(int j = 0; j<grid_size; j++){
                if(current_color<GRUVBOX_NUM_COLORS){
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
