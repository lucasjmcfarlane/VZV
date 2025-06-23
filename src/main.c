#include <raylib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION // Avoid including raygui implementation again

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

#include "gruvbox.h"
#include "constants.h"
#include "font_manager.h"
#include "text_renderer.h"

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
    SetTargetFPS(TARGET_FPS);

    char* placeholderText = "Open a file to get started...";

    char filePath[512] = "\0";
    char* loadedText = NULL;
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    fileDialogState.SelectFilePressed = false;
    fileDialogState.windowActive = true;

    // Font handling
    FontManager fontManager;
    InitFontManager(&fontManager);

    bool fontDropdownActive = false;
    
        }

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(Gruvbox[GRUVBOX_GREY]);

        // Draw toolbar
        DrawRectangle(0, 0, WINDOW_WIDTH, TOOLBAR_HEIGHT, Gruvbox[GRUVBOX_DARK1]);

        // Font dropdown (positioned on the right side)
        Rectangle fontDropdownRect = { WINDOW_WIDTH - 200, 5, 190, 30 };
        if (GuiDropdownBox(fontDropdownRect, fontManager.fontNames, &fontManager.currentFontIndex, fontDropdownActive)) {
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
        Font currentFont = fontManager.fonts[fontManager.currentFontIndex];

        if(loadedText){
            int lineNumber = 0;
            DrawTextWithCyclingColors(loadedText, &x, &y, startX, &lineNumber, currentFont);
        }
        else{
            DrawTextContiguous(placeholderText, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1, currentFont);
        }

        EndDrawing();
    }

    // Clean up
    if(loadedText){
        UnloadFileText(loadedText);
    }

    UnloadFontManager(&fontManager);

    CloseWindow();
    return 0;
}
