#include <raylib.h>
#include <gruvbox.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

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
#define LINE_NUMBER_GAP 50
#define TARGET_FPS 60

#define TOOLBAR_HEIGHT 40
#define MAX_FONTS 50
#define MAX_FONT_NAME_LENGTH 128
#define FONTS_DIR "fonts"
#define MAX_PATH_LENGTH 512

//print a line number
void DrawLineNumber(int lineNumber, float* x, float y, Color color, Font font){
    char lineNumberString[16];
    snprintf(lineNumberString, sizeof(lineNumberString), "%d", lineNumber);
    DrawTextEx(font, lineNumberString, (Vector2){*x, y}, FONT_SIZE, 1, BLACK);  // Use DrawTextEx for custom font
    *x += LINE_NUMBER_GAP;
}

//render text contiguously across multiple calls to the function
int DrawTextContiguous(const char* text, float* x, float* y, float startX, Color color, int lineNumber, Font font){
    if(text == NULL) return lineNumber;

    char buffer[1024];
    int bufferIndex = 0;

    if(lineNumber == 0){
        //draw the first line number if it hasn't been drawn already
        DrawLineNumber(++lineNumber, x, *y, BLACK, font);  // Pass font
    }

    for(int i = 0; ; i++){

        if(text[i] == '\n' || text[i] == '\0'){
            buffer[bufferIndex] = '\0';
            DrawTextEx(font, buffer, (Vector2){*x, *y}, FONT_SIZE, 1, color);  // Replace DrawText with DrawTextEx

            if(text[i] == '\n'){
                *x = startX;
                *y += FONT_SIZE + LINE_SPACING; //add some spacing between lines
                if(lineNumber != -1){
                    //draw the next line number if line numbers are enabled
                    DrawLineNumber(++lineNumber, x, *y, BLACK, font);  // Pass font
                }
                bufferIndex = 0;
            }

            else{
                *x += MeasureTextEx(font, buffer, FONT_SIZE, 1).x;  // Replace MeasureText with MeasureTextEx
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

// Update DrawTextWithCyclingColors to accept Font and pass it to DrawTextContiguous
void DrawTextWithCyclingColors(const char* text, float* x, float* y, float startX, int* lineNumber, Font font){
    if(text == NULL){
        DrawTextContiguous("Failed to load text from file!", x, y, startX, WHITE, -1, font);  // Pass font
        return;  // Early return for clarity
    }

    int colorIndex = 0;
    char word[512] = {'\0'};
    int wordLength = 0;

    for(int i = 0; text[i] != '\0'; i++){

        //add character to word
        if(wordLength < sizeof(word) - 1){
            word[wordLength++] = text[i];
        }

        //handle spaces, newlines, tabs, EOF as word delimiters
        if(text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i+1] == '\0'){
            //draw the completed word with the current color
            word[wordLength] = '\0';
            Color color = Gruvbox[colorIndex++ % GRUVBOX_NUM_COLORS];
            *lineNumber = DrawTextContiguous(word, x, y, startX, color, *lineNumber, font);
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

    // Font handling
    Font fonts[MAX_FONTS];
    char fontNames[MAX_FONTS* MAX_FONT_NAME_LENGTH] = {'\0'};
    int fontCount = 0;
    int currentFontIndex = 0;
    bool fontDropdownActive = false;
    
    // Add default font
    fonts[fontCount] = GetFontDefault();
    strcat(fontNames, "Default;");
    fontCount++;
    
    // Load fonts from fonts directory
    DIR* dir = opendir(FONTS_DIR);
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL && fontCount < MAX_FONTS) {
            if (entry->d_type == DT_REG) {
                const char* ext = strrchr(entry->d_name, '.');
                if (ext && (strcmp(ext, ".ttf") == 0 || strcmp(ext, ".otf") == 0)) {
                    char fontPath[MAX_PATH_LENGTH];
                    sprintf(fontPath, "%s/%s", FONTS_DIR, entry->d_name);
                    fonts[fontCount] = LoadFont(fontPath);
                    
                    // Remove extension for display name
                    strncat(fontNames, entry->d_name, MAX_FONT_NAME_LENGTH);
                    char* dot = strrchr(fontNames, '.');
                    *dot = ';';
                    *(dot+1) = '\0';

                    fontCount++;
                }
            }
        }
        closedir(dir);
    }
    //remove trailing ';'
    char* end = strrchr(fontNames, '\0');
    *(end-1) = '\0';

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(Gruvbox[GRUVBOX_GREY]);

        // Draw toolbar
        DrawRectangle(0, 0, WINDOW_WIDTH, TOOLBAR_HEIGHT, Gruvbox[GRUVBOX_DARK1]);

        // Font dropdown (positioned on the right side)
        Rectangle fontDropdownRect = { WINDOW_WIDTH - 200, 5, 190, 30 };
        if (GuiDropdownBox(fontDropdownRect, fontNames, &currentFontIndex, fontDropdownActive)) {
            fontDropdownActive = !fontDropdownActive;
        }

        float x = 0; // Starting x position (after the button)
        float y = TOOLBAR_HEIGHT + 5;  // Starting y position
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

        // Set the current font for drawing
        Font currentFont = fonts[currentFontIndex];

        if(loadedText){
            int lineNumber = 0;
            DrawTextWithCyclingColors(loadedText, &x, &y, startX, &lineNumber, currentFont);
        }
        else{
            DrawTextContiguous(text1, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1, currentFont);
            DrawTextContiguous(text2, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1, currentFont);
            DrawTextContiguous(text3, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1, currentFont);
        }

        EndDrawing();
    }

    // Clean up
    if(loadedText){
        UnloadFileText(loadedText);
    }

    // Unload fonts (except default font which doesn't need to be unloaded)
    for (int i = 1; i < fontCount; i++) {
        UnloadFont(fonts[i]);
    }

    CloseWindow();
    return 0;
}
