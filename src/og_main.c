#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define APPLICATION_NAME "RaylibEditor"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000
#define FONT_SIZE 25
#define TARGET_FPS 60

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);
    GuiLoadStyle("styles/jungle/style_jungle.rgs");
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
    SetTargetFPS(TARGET_FPS);

    char* text1 = "Hello, World! ";
    char* text2 = "Some more text. ";
    char* text3 = "This text should\nStart again from the beginning.";

    //hide compiler warning
    (void)text1;
    (void)text2;
    (void)text3;

    bool toggle = false;

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        if(GuiButton((Rectangle){0,0,200,100}, "Press Me!")){
            toggle = !toggle;
        }

        if(toggle){
            //text should be drawn when toggle is true
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
