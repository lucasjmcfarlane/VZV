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

    // Scrolling variables
    float scrollY = 0.0f;
    float maxScrollY = 0.0f;
    float contentHeight = 0.0f;
    float viewportHeight = WINDOW_HEIGHT - TOOLBAR_HEIGHT - 10; // Available height for text
    const float scrollSpeed = FONT_SIZE + TEXT_LINE_SPACING;
    const int scrollbarWidth = 20;
    
    while(!WindowShouldClose()){
        // Handle scrolling input
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            scrollY -= wheel * scrollSpeed;
            if (scrollY < 0) scrollY = 0;
            if (scrollY > maxScrollY) scrollY = maxScrollY;
        }

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
        float y = TOOLBAR_HEIGHT + 5 - scrollY;  // Apply scroll offset
        float startX = x; // Remember the starting x for newlines
        float originalY = y; // Remember original Y to calculate content height
        
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

        // Create a scissor rectangle to clip text rendering to the viewport
        BeginScissorMode(0, TOOLBAR_HEIGHT, WINDOW_WIDTH - scrollbarWidth, viewportHeight);

        if(loadedText){
            int lineNumber = 0;
            DrawTextWithCyclingColors(loadedText, &x, &y, startX, &lineNumber, currentFont);
        }
        else{
            DrawTextContiguous(placeholderText, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1, currentFont);
        }

        EndScissorMode();

        // Calculate content height and max scroll
        contentHeight = y - originalY;
        maxScrollY = contentHeight > viewportHeight ? contentHeight - viewportHeight : 0;

        // Draw scrollbar if content exceeds viewport
        if (maxScrollY > 0) {
            Rectangle scrollbarBg = { WINDOW_WIDTH - scrollbarWidth, TOOLBAR_HEIGHT, scrollbarWidth, viewportHeight };
            DrawRectangleRec(scrollbarBg, Gruvbox[GRUVBOX_DARK2]);
            
            // Calculate scrollbar thumb size and position
            float thumbHeight = (viewportHeight / contentHeight) * viewportHeight;
            if (thumbHeight < 20) thumbHeight = 20; // Minimum thumb size
            
            float thumbY = TOOLBAR_HEIGHT + (scrollY / maxScrollY) * (viewportHeight - thumbHeight);
            
            Rectangle scrollbarThumb = { WINDOW_WIDTH - scrollbarWidth + 2, thumbY, scrollbarWidth - 4, thumbHeight };
            DrawRectangleRec(scrollbarThumb, Gruvbox[GRUVBOX_BRIGHT_BLUE]);
            
            // Handle scrollbar dragging
            static bool dragging = false;
            static float dragOffset = 0;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, scrollbarThumb)) {
                    dragging = true;
                    dragOffset = mousePos.y - thumbY;
                }
            }
            
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                dragging = false;
            }
            
            if (dragging) {
                Vector2 mousePos = GetMousePosition();
                float newThumbY = mousePos.y - dragOffset;
                
                // Constrain thumb position
                if (newThumbY < TOOLBAR_HEIGHT) newThumbY = TOOLBAR_HEIGHT;
                if (newThumbY > TOOLBAR_HEIGHT + viewportHeight - thumbHeight) {
                    newThumbY = TOOLBAR_HEIGHT + viewportHeight - thumbHeight;
                }
                
                // Update scroll position based on thumb position
                float thumbProgress = (newThumbY - TOOLBAR_HEIGHT) / (viewportHeight - thumbHeight);
                scrollY = thumbProgress * maxScrollY;
            }
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
