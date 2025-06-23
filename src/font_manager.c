#include "font_manager.h"
#include <string.h>
#include <dirent.h>
#include <stdio.h>

void InitFontManager(FontManager* man) {
    man->fontCount = 0;
    man->currentFontIndex = 0;
    memset(man->fontNames, 0, MAX_FONT_NAMES_STRING);

    man->fonts[man->fontCount++] = GetFontDefault();
    strcat(man->fontNames, "Default;");

    DIR* dir = opendir(FONTS_DIR);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && man->fontCount < MAX_FONTS) {
        if (entry->d_type != DT_REG) continue;

        const char* ext = strrchr(entry->d_name, '.');
        if (!ext || (strcmp(ext, ".ttf") != 0 && strcmp(ext, ".otf") != 0)) continue;

        char fontPath[MAX_FONT_PATH_LENGTH];
        snprintf(fontPath, sizeof(fontPath), "%s/%s", FONTS_DIR, entry->d_name);

        Font font = LoadFont(fontPath);
        man->fonts[man->fontCount++] = font;

        char nameOnly[MAX_FONT_NAME_LENGTH];
        strncpy(nameOnly, entry->d_name, MAX_FONT_NAME_LENGTH);
        char* dot = strrchr(nameOnly, '.');
        if (dot) *dot = '\0';

        strncat(man->fontNames, nameOnly, MAX_FONT_NAME_LENGTH);
        strcat(man->fontNames, ";");
    }

    closedir(dir);

    size_t len = strlen(man->fontNames);
    if (len > 0 && man->fontNames[len - 1] == ';') {
        man->fontNames[len - 1] = '\0';
    }
}

void UnloadFontManager(FontManager* man) {
    for (int i = 1; i < man->fontCount; i++) {
        UnloadFont(man->fonts[i]);
    }
    man->fontCount = 0;
}
