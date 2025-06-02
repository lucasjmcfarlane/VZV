#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define APPLICATION_NAME "RaylibEditor"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000
#define FONT_SIZE 25
#define TARGET_FPS 60

// Rainbow colors array (ROYGBIV)
Color rainbowColors[7] = {
    RED,        // Red
    ORANGE,     // Orange
    YELLOW,     // Yellow
    GREEN,      // Green
    BLUE,       // Blue
    (Color){75,0,130,255}, //Indigo (custom color since Raylib lacks a predefined one)
    VIOLET      // Violet
};

// Function to render text contiguously with rainbow colors
void DrawRainbowText(const char* text, float* x, float* y, float startX, int* colorIndex){
    for(int i = 0; text[i] != '\0'; i++){
        if(text[i] == '\n'){
            // Handle newline: reset x to starting position and move y down
            *x = startX;
            *y += FONT_SIZE + 5; // Add some spacing between lines
        }
        else{
            // Draw the character with current rainbow color
            char charStr[2] = {text[i], '\0'};
            DrawText(charStr, *x, *y, FONT_SIZE, rainbowColors[*colorIndex]);
            
            // Move to next position and next color
            *x += MeasureText(charStr, FONT_SIZE);
            *colorIndex = (*colorIndex + 1) % 7; // Cycle through rainbow colors
        }
    }
}

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);
    GuiLoadStyle("styles/jungle/style_jungle.rgs");
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
    SetTargetFPS(TARGET_FPS);

    char* text1 = "Hello, World! ";
    char* text2 = "Some more text. ";
    char* text3 = "This text should\nStart again from the beginning.";

    bool toggle = false;

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        if(GuiButton((Rectangle){0,0,200,100}, "Press Me!")){
            toggle = !toggle;
        }

        if(toggle){
            // Draw the text strings contiguously with rainbow colors
            float x = 220; // Starting x position (after the button)
            float y = 50;  // Starting y position
            float startX = x; // Remember the starting x for newlines
            int colorIndex = 0; // Start with the first rainbow color
            
            // Draw all three text strings contiguously
            DrawRainbowText(text1, &x, &y, startX, &colorIndex);
            DrawRainbowText(text2, &x, &y, startX, &colorIndex);
            DrawRainbowText(text3, &x, &y, startX, &colorIndex);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
