#include <raylib.h>
#include <gruvbox.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION // Avoid including raygui implementation again

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

#define APPLICATION_NAME "VZV"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000
#define FONT_SIZE 25
#define LINE_SPACING 5
#define TARGET_FPS 60

// Function to render text contiguously with rainbow colors
void DrawTextContiguous(const char* text, float* x, float* y, float startX, Color color){
    //loop until end of string
    for(int i = 0; text[i] != '\0'; i++){

        // Handle newline: reset x to startX and move y down
        if(text[i] == '\n'){
            *x = startX;
            *y += FONT_SIZE + LINE_SPACING; // Add some spacing between lines
        }

        else{
            // Draw the character with appropriate color
            char charStr[2] = {text[i], '\0'};
            DrawText(charStr, *x, *y, FONT_SIZE, color);
            
            // Move to next position and next color
            *x += MeasureText(charStr, FONT_SIZE);
        }
    }
}

void DrawTextWithCyclingColors(const char* text, float* x, float* y, float startX){
    if(text == NULL){
        DrawTextContiguous("Failed to load text from file!", x, y, startX, WHITE);
    }

    int colorIndex = 0;
    char word[512] = {'\0'};
    int wordLength = 0;

    for(int i = 0; text[i] != '\0'; i++){

        // Add character to word
        if(wordLength < sizeof(word) - 1){
            word[wordLength++] = text[i];
        }

        // Handle whitespace (spaces, newlines, tabs, EOF) as word delimiters
        if(text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i+1] == '\0'){
            // Draw the completed word with the current color
            word[wordLength] = '\0';
            Color color = Gruvbox[colorIndex++ % GRUVBOX_NUM_COLORS];
            DrawTextContiguous(word, x, y, startX, color);
            wordLength = 0;
        }
    }
}

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
    SetTargetFPS(TARGET_FPS);

    char* text1 = "Hello, World! ";
    char* text2 = "Some more text. ";
    char* text3 = "This text should\nStart again from the beginning.";

    char filePath[512] = "\0";
    char* loadedText = NULL;
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    fileDialogState.SelectFilePressed = false;
    fileDialogState.windowActive = true;

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(Gruvbox[GRUVBOX_GREY]);

        float x = 0; // Starting x position (after the button)
        float y = 0;  // Starting y position
        float startX = x; // Remember the starting x for newlines
        
        // Display the file dialog
        GuiWindowFileDialog(&fileDialogState);

        // Handle file dialog
        if(fileDialogState.SelectFilePressed){
            TextCopy(filePath, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
            loadedText = LoadFileText(filePath);
            fileDialogState.SelectFilePressed = false;
            fileDialogState.windowActive = false;
        }

        if(loadedText){
            DrawTextWithCyclingColors(loadedText, &x, &y, startX);
        }
        else{
            DrawTextContiguous(text1, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA]);
            DrawTextContiguous(text2, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA]);
            DrawTextContiguous(text3, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA]);
        }

        EndDrawing();
    }

    // Clean up
    if(loadedText){
        UnloadFileText(loadedText);
    }

    CloseWindow();
    return 0;
}
