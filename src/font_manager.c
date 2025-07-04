#include "font_manager.h"
#include <string.h>
#include <dirent.h>
#include <stdio.h>

void InitFontManager(FontManager* man) {
    //initialize struct
    man->fontCount = 0;
    man->currentFontIndex = 0;
    memset(man->fontNames, 0, MAX_FONT_NAMES_STRING);
    // ... existing code ...

    //add default font
    man->fonts[man->fontCount++] = GetFontDefault();
    strcat(man->fontNames, "Default;");
    strcpy(man->individualFontNames[0], "Default"); // Store individual name

    //try to open fonts directory
    DIR* dir = opendir(FONTS_DIR);
    if (!dir) return;

    //iterate through directory entries
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && man->fontCount < MAX_FONTS) {

        if(entry->d_type == DT_REG){

            const char* ext = strrchr(entry->d_name, '.');

            if(ext && (strcmp(ext, ".ttf") == 0 || strcmp(ext, ".otf") == 0)){

                //entry is a ttf or otf
                //get path to font
                char fontPath[MAX_FONT_PATH_LENGTH];
                snprintf(fontPath, sizeof(fontPath), "%s/%s", FONTS_DIR, entry->d_name);

                //add Font to array
                Font font = LoadFont(fontPath);
                man->fonts[man->fontCount] = font;

                //add name to fontNames array
                char nameOnly[MAX_FONT_NAME_LENGTH];
                strncpy(nameOnly, entry->d_name, MAX_FONT_NAME_LENGTH);
                char* dot = strrchr(nameOnly, '.');
                if (dot) *dot = '\0';
                
                // Store individual name
                strcpy(man->individualFontNames[man->fontCount], nameOnly);
                
                strncat(man->fontNames, nameOnly, MAX_FONT_NAME_LENGTH);
                strcat(man->fontNames, ";");
                
                man->fontCount++;
            }
        }
    }
    closedir(dir);

    //remove trailing semicolon
    size_t len = strlen(man->fontNames);
    if (len > 0 && man->fontNames[len - 1] == ';') {
        man->fontNames[len - 1] = '\0';
    }
}

// Custom dropdown that renders each font name in its own font
bool DrawCustomFontDropdown(Rectangle bounds, FontManager* fontManager, bool* dropdownOpen) {
    bool selectionChanged = false;
    
    // Draw the main dropdown button
    if (GuiDropdownBox(bounds, fontManager->fontNames, &fontManager->currentFontIndex, false)) {
        *dropdownOpen = !*dropdownOpen;
    }
    
    // If dropdown is open, draw custom font list
    if (*dropdownOpen) {
        float itemHeight = 25;
        Rectangle listBounds = { bounds.x, bounds.y + bounds.height, bounds.width, fontManager->fontCount * itemHeight };
        
        // Draw background for dropdown list
        DrawRectangleRec(listBounds, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        DrawRectangleLinesEx(listBounds, 1, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));
        
        // Draw each font option using its own font
        for (int i = 0; i < fontManager->fontCount; i++) {
            Rectangle itemBounds = { bounds.x, bounds.y + bounds.height + (i * itemHeight), bounds.width, itemHeight };
            
            // Check if mouse is over this item
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), itemBounds);
            bool isSelected = (i == fontManager->currentFontIndex);
            
            // Draw item background
            Color bgColor = isSelected ? GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED)) :
                           isHovered ? GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED)) :
                           GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
            DrawRectangleRec(itemBounds, bgColor);
            
            // Draw text using the font's own typeface
            Font fontToUse = fontManager->fonts[i];
            Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
            
            // Calculate text position (centered vertically)
            Vector2 textSize = MeasureTextEx(fontToUse, fontManager->individualFontNames[i], 16, 1);
            Vector2 textPos = {
                itemBounds.x + 5,
                itemBounds.y + (itemBounds.height - textSize.y) / 2
            };
            
            DrawTextEx(fontToUse, fontManager->individualFontNames[i], textPos, 16, 1, textColor);
            
            // Handle click
            if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                fontManager->currentFontIndex = i;
                *dropdownOpen = false;
                selectionChanged = true;
            }
        }
    }
    
    return selectionChanged;
}

//free memory allocated for fonts
void UnloadFontManager(FontManager* man) {
    for (int i = 1; i < man->fontCount; i++) {
        UnloadFont(man->fonts[i]);
    }
    man->fontCount = 0;
}
