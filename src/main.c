#include <raylib.h>
#include <dirent.h>

#include "raygui.h"
#include "gruvbox.h"
#include "constants.h"
#include "font_manager.h"
#include "text_renderer.h"
#include "file_dialog_manager.h"
#include "scroll_manager.h"
#include "palette.h"

enum{
    CHANGE_BACKGROUND_COLOR,
    CHANGE_FONT_COLOR,
};


int main(){
    bool fontDropdownOpen = false;
    bool textWrapEnabled = false;
    bool paletteOpen = false;

    int currentBackgroundIndex = DEFAULT_BACKGROUND_COLOR;
    int currentFontColorIndex = DEFAULT_FONT_COLOR;
    int paletteMode = CHANGE_BACKGROUND_COLOR;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPLICATION_NAME);

    FileDialogManager fileDialogManager;
    InitFileDialogManager(&fileDialogManager);

    FontManager fontManager;
    InitFontManager(&fontManager);

    ScrollManager scrollManager;
    InitScrollManager(&scrollManager);

    while(!WindowShouldClose()){

        //-------------------------------------------------------------KEYBOARD SHORTCUTS---------------------------------------
        //open file picker
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) {
            ShowFileDialog(&fileDialogManager);
        }

        //close the app
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) {
            break;
        }

        //-------------------------------------------------------------CALCULATIONS---------------------------------------------
        //determine if scrollbars should be shown
        bool showVerticalScrollbar = (scrollManager.maxScrollY > 0);
        bool showHorizontalScrollbar = (scrollManager.maxScrollX > 0 && !textWrapEnabled);

        //calculate size of the textbox
        float textboxWidth = WINDOW_WIDTH - NUMBER_LINE_WIDTH - (showVerticalScrollbar ? SCROLLBAR_WIDTH : 0);
        float textboxHeight = WINDOW_HEIGHT - TOOLBAR_HEIGHT - (showHorizontalScrollbar ? SCROLLBAR_WIDTH : 0);

        //get current scroll offset
        HandleScrollInput(&scrollManager, textWrapEnabled, textboxWidth, textboxHeight);
        Vector2 scrollOffset = GetScrollOffset(&scrollManager);

        //starting x pos of text content
        float x = scrollOffset.x;
        //starting y pos of text content
        float y = TOOLBAR_HEIGHT + scrollOffset.y;
        //keep for resetting newlines to the correct x pos
        const float startX = x;
        //keep for calculating total content length
        const float startY = y;
        //length of the text content 
        float maxX = 0;
        
        // Set the current font
        Font currentFont = fontManager.fonts[fontManager.currentFontIndex];

        // Check if a new file was selected
        if (IsFileSelected(&fileDialogManager)) {
            // Reset scroll position when a new file is loaded
            ResetScrollPosition(&scrollManager);
        }

        //-------------------------------------------------------------DRAW TEXT------------------------------------------------
        BeginDrawing();
        ClearBackground(Gruvbox[currentBackgroundIndex]);

        //clip text rendering to the textbox
        BeginScissorMode(NUMBER_LINE_WIDTH, TOOLBAR_HEIGHT, textboxWidth, textboxHeight);

        char* loadedText = GetLoadedText(&fileDialogManager);
        if(loadedText){
            float wrapWidth = textWrapEnabled ? textboxWidth : 0; // 0 means no wrapping
            DrawTextContiguous(loadedText, &x, &y, startX, Gruvbox[currentFontColorIndex], 0, currentFont, textWrapEnabled, wrapWidth, &maxX, textboxWidth, textboxHeight);
        }
        else{
            DrawTextContiguous(PLACEHOLDER_TEXT, &x, &y, startX, Gruvbox[currentFontColorIndex], -1, currentFont, textWrapEnabled, 0, &maxX, textboxWidth, textboxHeight);
        }

        EndScissorMode();

        //-------------------------------------------------------------DRAW UI--------------------------------------------------
        //toolbar rectangle
        DrawRectangle(0, 0, WINDOW_WIDTH, TOOLBAR_HEIGHT, Gruvbox[GRUVBOX_DARK1]);

        //open file button
        Rectangle openFileRect = { 10, 5, 80, 30 };
        if (GuiButton(openFileRect, "Open File")) {
            ShowFileDialog(&fileDialogManager);
        }

        //text wrap toggle button
        Rectangle wrapToggleRect = { 100, 5, 120, 30 };
        if (GuiButton(wrapToggleRect, textWrapEnabled ? "Wrap: ON" : "Wrap: OFF")) {
            textWrapEnabled = !textWrapEnabled;
        }

        //font dropdown
        Rectangle fontDropdownRect = { WINDOW_WIDTH - 200, 5, 190, 30 };
        if (GuiDropdownBox(fontDropdownRect, fontManager.fontNames, &fontManager.currentFontIndex, fontDropdownOpen)) {
            fontDropdownOpen = !fontDropdownOpen;
        }

        //background color button
        Rectangle bgColorRect = {WINDOW_WIDTH - 400, 5, 90, 30};
        if (GuiButton(bgColorRect, "Background")) {
            paletteOpen = true;
            paletteMode = 0;
        }

        //font color button
        Rectangle fontColorRect = {WINDOW_WIDTH - 300, 5, 90, 30};
        if (GuiButton(fontColorRect, "Font Color")) {
            paletteOpen = true;
            paletteMode = 1;
        }

        //palette
        if(paletteOpen){
            if(paletteMode == 0){
                int newColor = Palette("Choose Background Color...");
                currentBackgroundIndex = newColor > -1 ? newColor : currentBackgroundIndex;
                paletteOpen = newColor != -1 ? false : true;
            }
            else if (paletteMode == 1){
                int newColor = Palette("Choose Font Color...");
                currentFontColorIndex = newColor > -1 ? newColor : currentFontColorIndex;
                paletteOpen = newColor != -1 ? false : true;
            }
        }

        //scrollbars
        UpdateScrollBounds(&scrollManager, y-startY, maxX-startX, textboxWidth, textboxHeight, textWrapEnabled);
        DrawScrollbars(&scrollManager, textboxWidth, textboxHeight, TOOLBAR_HEIGHT, NUMBER_LINE_WIDTH, SCROLLBAR_WIDTH);

        //file dialog
        UpdateFileDialog(&fileDialogManager);

        EndDrawing();
    }

    // Clean up
    UnloadFileDialogManager(&fileDialogManager);
    UnloadFontManager(&fontManager);
    CloseWindow();
    return 0;
}
