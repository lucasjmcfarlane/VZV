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

    // Text wrapping toggle
    bool textWrapEnabled = true;

    // Scrolling variables
    float scrollY = 0.0f;
    float scrollX = 0.0f;
    float maxScrollY = 0.0f;
    float maxScrollX = 0.0f;
    float contentHeight = 0.0f;
    float contentWidth = 0.0f;
    
    while(!WindowShouldClose()){
        bool showVerticalScrollbar = (maxScrollY > 0);
        float viewportWidth = WINDOW_WIDTH - NUMBER_LINE_WIDTH - 10 - (showVerticalScrollbar ? SCROLLBAR_WIDTH : 0);

        bool showHorizontalScrollbar = (maxScrollX > 0 && !textWrapEnabled);
        float viewportHeight = WINDOW_HEIGHT - TOOLBAR_HEIGHT - 10 - (showHorizontalScrollbar ? SCROLLBAR_WIDTH : 0);

        // Handle scrolling input
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            if (IsKeyDown(KEY_LEFT_SHIFT) && !textWrapEnabled) {
                // Horizontal scrolling when shift is held and wrapping is off
                scrollX -= wheel * SCROLL_SPEED;
                if (scrollX < 0) scrollX = 0;
                if (scrollX > maxScrollX) scrollX = maxScrollX;
            } else {
                // Vertical scrolling
                scrollY -= wheel * SCROLL_SPEED;
                if (scrollY < 0) scrollY = 0;
                if (scrollY > maxScrollY) scrollY = maxScrollY;
            }
        }

        BeginDrawing();
        ClearBackground(Gruvbox[GRUVBOX_GREY]);


        float x = -scrollX; // Apply horizontal scroll offset
        float y = TOOLBAR_HEIGHT + 5 - scrollY;  // Apply vertical scroll offset
        float startX = x; // Remember the starting x for newlines
        float originalY = y; // Remember original Y to calculate content height
        float maxX = 0; // Track maximum X position for horizontal scrolling
        
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
        BeginScissorMode(NUMBER_LINE_WIDTH, TOOLBAR_HEIGHT, viewportWidth, viewportHeight);

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

        // Text wrap toggle button
        Rectangle wrapToggleRect = { 10, 5, 120, 30 };
        if (GuiButton(wrapToggleRect, textWrapEnabled ? "Wrap: ON" : "Wrap: OFF")) {
            textWrapEnabled = !textWrapEnabled;
            // Reset horizontal scroll when enabling wrap
            if (textWrapEnabled) {
                scrollX = 0;
                maxScrollX = 0;
            }
        }

        // Font dropdown (positioned on the right side)
        Rectangle fontDropdownRect = { WINDOW_WIDTH - 200, 5, 190, 30 };
        if (GuiDropdownBox(fontDropdownRect, fontManager.fontNames, &fontManager.currentFontIndex, fontDropdownActive)) {
            fontDropdownActive = !fontDropdownActive;
        }

        // Calculate content dimensions and max scroll
        contentHeight = y - originalY;
        contentWidth = maxX + scrollX;
        maxScrollY = contentHeight > viewportHeight ? contentHeight - viewportHeight : 0;
        maxScrollX = (!textWrapEnabled && contentWidth > viewportWidth) ? contentWidth - viewportWidth : 0;

        // Draw vertical scrollbar if content exceeds viewport
        if (maxScrollY > 0) {
            Rectangle scrollbarBg = { WINDOW_WIDTH - SCROLLBAR_WIDTH, TOOLBAR_HEIGHT, SCROLLBAR_WIDTH, viewportHeight - (maxScrollX > 0 ? SCROLLBAR_WIDTH : 0) };
            DrawRectangleRec(scrollbarBg, Gruvbox[GRUVBOX_DARK2]);

            // Calculate scrollbar thumb size and position
            float thumbHeight = (viewportHeight / contentHeight) * scrollbarBg.height;
            if (thumbHeight < 20) thumbHeight = 20; // Minimum thumb size

            float thumbY = TOOLBAR_HEIGHT + (scrollY / maxScrollY) * (scrollbarBg.height - thumbHeight);

            Rectangle scrollbarThumb = { WINDOW_WIDTH - SCROLLBAR_WIDTH + 2, thumbY, SCROLLBAR_WIDTH - 4, thumbHeight };
            DrawRectangleRec(scrollbarThumb, Gruvbox[GRUVBOX_BRIGHT_BLUE]);

            // Handle vertical scrollbar dragging
            static bool draggingVertical = false;
            static float dragOffsetVertical = 0;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, scrollbarThumb)) {
                    draggingVertical = true;
                    dragOffsetVertical = mousePos.y - thumbY;
                }
            }

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                draggingVertical = false;
            }

            if (draggingVertical) {
                Vector2 mousePos = GetMousePosition();
                float newThumbY = mousePos.y - dragOffsetVertical;

                // Constrain thumb position
                if (newThumbY < TOOLBAR_HEIGHT) newThumbY = TOOLBAR_HEIGHT;
                if (newThumbY > TOOLBAR_HEIGHT + scrollbarBg.height - thumbHeight) {
                    newThumbY = TOOLBAR_HEIGHT + scrollbarBg.height - thumbHeight;
                }

                // Update scroll position based on thumb position
                float thumbProgress = (newThumbY - TOOLBAR_HEIGHT) / (scrollbarBg.height - thumbHeight);
                scrollY = thumbProgress * maxScrollY;
            }
        }

        // Draw horizontal scrollbar if content exceeds viewport and wrapping is off
        if (maxScrollX > 0 && !textWrapEnabled) {
            Rectangle hScrollbarBg = {
                NUMBER_LINE_WIDTH,
                WINDOW_HEIGHT - SCROLLBAR_WIDTH,
                viewportWidth,
                SCROLLBAR_WIDTH
            };
            DrawRectangleRec(hScrollbarBg, Gruvbox[GRUVBOX_DARK2]);

            float thumbWidth = (viewportWidth / contentWidth) * hScrollbarBg.width;
            if (thumbWidth < 20) thumbWidth = 20; // Minimum thumb size

            float thumbX = hScrollbarBg.x + (scrollX / maxScrollX) * (hScrollbarBg.width - thumbWidth);

            Rectangle hScrollbarThumb = {
                thumbX,
                hScrollbarBg.y + 2,
                thumbWidth,
                hScrollbarBg.height - 4
            };
            DrawRectangleRec(hScrollbarThumb, Gruvbox[GRUVBOX_BRIGHT_BLUE]);

            static bool draggingHorizontal = false;
            static float dragOffsetHorizontal = 0;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, hScrollbarThumb)) {
                    draggingHorizontal = true;
                    dragOffsetHorizontal = mousePos.x - thumbX;
                }
            }

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                draggingHorizontal = false;
            }

            if (draggingHorizontal) {
                Vector2 mousePos = GetMousePosition();
                float newThumbX = mousePos.x - dragOffsetHorizontal;

                // Constrain thumb within scrollbar background
                if (newThumbX < hScrollbarBg.x) newThumbX = hScrollbarBg.x;
                if (newThumbX > hScrollbarBg.x + hScrollbarBg.width - thumbWidth) {
                    newThumbX = hScrollbarBg.x + hScrollbarBg.width - thumbWidth;
                }

                float thumbProgress = (newThumbX - hScrollbarBg.x) / (hScrollbarBg.width - thumbWidth);
                scrollX = thumbProgress * maxScrollX;
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
