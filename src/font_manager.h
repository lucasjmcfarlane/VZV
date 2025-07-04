#pragma once
#include <raylib.h>
#include "raygui.h"

#define MAX_FONTS 64
#define MAX_FONT_NAME_LENGTH 128
#define MAX_FONT_PATH_LENGTH 512
#define MAX_FONT_NAMES_STRING (MAX_FONTS * MAX_FONT_NAME_LENGTH)
#define FONTS_DIR "fonts"

typedef struct {
    Font fonts[MAX_FONTS];
    char fontNames[MAX_FONT_NAMES_STRING];
    char individualFontNames[MAX_FONTS][MAX_FONT_NAME_LENGTH]; // Add this for individual access
    int fontCount;
    int currentFontIndex;
} FontManager;

void InitFontManager(FontManager* man);
void UnloadFontManager(FontManager* man);
bool DrawCustomFontDropdown(Rectangle bounds, FontManager* fontManager, bool* dropdownOpen);
