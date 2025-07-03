// src/main.c
#include <raylib.h>
#include <dirent.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION // Avoid including raygui implementation again

#include "gruvbox.h"
#include "constants.h"
#include "font_manager.h"
#include "text_renderer.h"
#include "file_dialog_manager.h"
#include "scroll_manager.h"

#define APPLICATION_NAME "VZV"

int main(){
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);//set font size
    SetTargetFPS(TARGET_FPS);

    const char* placeholderText = "Open a file to get started...";

    // File dialog manager
    FileDialogManager fileDialogManager;
    InitFileDialogManager(&fileDialogManager);

    // Font manager
    FontManager fontManager;
    InitFontManager(&fontManager);

    // Scroll manager
    ScrollManager scrollManager;
    InitScrollManager(&scrollManager);

    //font dropdown toggle
    bool fontDropdownActive = false;

    // Text wrapping toggle
    bool textWrapEnabled = true;
    
    while(!WindowShouldClose()){
        // Handle keyboard shortcuts
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) {
            ShowFileDialog(&fileDialogManager);
        }

        bool showVerticalScrollbar = (scrollManager.maxScrollY > 0);
        float viewportWidth = WINDOW_WIDTH - NUMBER_LINE_WIDTH - 10 - (showVerticalScrollbar ? SCROLLBAR_WIDTH : 0);

        bool showHorizontalScrollbar = (scrollManager.maxScrollX > 0 && !textWrapEnabled);
        float viewportHeight = WINDOW_HEIGHT - TOOLBAR_HEIGHT - 10 - (showHorizontalScrollbar ? SCROLLBAR_WIDTH : 0);

        // Handle scrolling input
        HandleScrollInput(&scrollManager, textWrapEnabled, viewportWidth, viewportHeight);

        BeginDrawing();
        ClearBackground(Gruvbox[GRUVBOX_GREY]);

        Vector2 scrollOffset = GetScrollOffset(&scrollManager);
        float x = scrollOffset.x; // Apply horizontal scroll offset
        float y = TOOLBAR_HEIGHT + 5 + scrollOffset.y;  // Apply vertical scroll offset
        float startX = x; // Remember the starting x for newlines
        float originalY = y; // Remember original Y to calculate content height
        float maxX = 0; // Track maximum X position for horizontal scrolling
        
        // Update file dialog
        UpdateFileDialog(&fileDialogManager);

        // Check if a new file was selected
        if (IsFileSelected(&fileDialogManager)) {
            // Reset scroll position when a new file is loaded
            ResetScrollPosition(&scrollManager);
        }

        // Set the current font for drawing
        Font currentFont = fontManager.fonts[fontManager.currentFontIndex];

        // Create a scissor rectangle to clip text rendering to the viewport
        BeginScissorMode(NUMBER_LINE_WIDTH, TOOLBAR_HEIGHT, viewportWidth, viewportHeight);

        char* loadedText = GetLoadedText(&fileDialogManager);
        if(loadedText){
            int lineNumber = 0;
            float wrapWidth = textWrapEnabled ? viewportWidth : 0; // 0 means no wrapping
            DrawTextWithCyclingColors(loadedText, &x, &y, startX, &lineNumber, currentFont, textWrapEnabled, wrapWidth, &maxX, viewportWidth, viewportHeight);
        }
        else{
            float wrapWidth = textWrapEnabled ? viewportWidth : 0;
            DrawTextContiguous(placeholderText, &x, &y, startX, Gruvbox[GRUVBOX_BRIGHT_AQUA], -1, currentFont, textWrapEnabled, wrapWidth, &maxX, viewportWidth, viewportHeight);
        }

        EndScissorMode();

        // Draw toolbar
        DrawRectangle(0, 0, WINDOW_WIDTH, TOOLBAR_HEIGHT, Gruvbox[GRUVBOX_DARK1]);

        // Open file button
        Rectangle openFileRect = { 10, 5, 80, 30 };
        if (GuiButton(openFileRect, "Open File")) {
            ShowFileDialog(&fileDialogManager);
        }

        // Text wrap toggle button
        Rectangle wrapToggleRect = { 100, 5, 120, 30 };
        if (GuiButton(wrapToggleRect, textWrapEnabled ? "Wrap: ON" : "Wrap: OFF")) {
            textWrapEnabled = !textWrapEnabled;
        }

        // Font dropdown (positioned on the right side)
        Rectangle fontDropdownRect = { WINDOW_WIDTH - 200, 5, 190, 30 };
        if (GuiDropdownBox(fontDropdownRect, fontManager.fontNames, &fontManager.currentFontIndex, fontDropdownActive)) {
            fontDropdownActive = !fontDropdownActive;
        }

        // Update scroll bounds based on content dimensions
        float contentHeight = y - originalY;
        float contentWidth = maxX - scrollOffset.x;
        UpdateScrollBounds(&scrollManager, contentHeight, contentWidth, viewportWidth, viewportHeight, textWrapEnabled);

        // Draw scrollbars
        DrawScrollbars(&scrollManager, viewportWidth, viewportHeight, TOOLBAR_HEIGHT, NUMBER_LINE_WIDTH, SCROLLBAR_WIDTH);

        EndDrawing();
    }

    // Clean up
    UnloadFileDialogManager(&fileDialogManager);
    UnloadFontManager(&fontManager);

    CloseWindow();
    return 0;
}
