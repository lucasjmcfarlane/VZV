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
#define LINE_NUMBER_GAP 30
#define TARGET_FPS 60


void DrawLineNumber(int lineNumber, float* x, float y, Color color){
    char lineNumberString[16];
    snprintf(lineNumberString, sizeof(lineNumberString), "%d", lineNumber);
    DrawText(lineNumberString, *x, y, FONT_SIZE, BLACK);
    *x += MeasureText(lineNumberString, FONT_SIZE) + LINE_NUMBER_GAP;
}


// Function to render text contiguously
int DrawTextContiguous(const char* text, float* x, float* y, float startX, Color color, int lineNumber){
    if(text == NULL) return lineNumber;

    char buffer[1024];
    int bufferIndex = 0;

    if(lineNumber == 0){
        // Draw the first line number if it hasn't been drawn already
        DrawLineNumber(++lineNumber, x, *y, BLACK);
    }

    for(int i = 0; ; i++){

        if(text[i] == '\n' || text[i] == '\0'){
            buffer[bufferIndex] = '\0';
            DrawText(buffer, *x, *y, FONT_SIZE, color);

            if(text[i] == '\n'){
                *x = startX;
                *y += FONT_SIZE + LINE_SPACING; // Add some spacing between lines
                if(lineNumber != -1){
                    // Draw the next line number if line numbers are enabled
                    DrawLineNumber(++lineNumber, x, *y, BLACK);
                }
                bufferIndex = 0;
            }

            else{
                *x += MeasureText(buffer, FONT_SIZE);
                break;
            }
        }

        else{
            if(bufferIndex < sizeof(buffer) - 1){
                buffer[bufferIndex++] = text[i];
            }
        }
    }
    return lineNumber;
}

void DrawTextWithCyclingColors(const char* text, float* x, float* y, float startX, int* lineNumber){
    if(text == NULL){
        DrawTextContiguous("Failed to load text from file!", x, y, startX, WHITE, -1);
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
            *lineNumber = DrawTextContiguous(word, x, y, startX, color, *lineNumber);
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
            int lineNumber = 0;
            DrawTextWithCyclingColors(loadedText, &x, &y, startX, &lineNumber);
        }
        else{
            DrawTextContiguous(text1, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1);
            DrawTextContiguous(text2, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1);
            DrawTextContiguous(text3, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1);
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
