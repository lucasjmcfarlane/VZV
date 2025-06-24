#pragma once

#include <raylib.h>

int DrawTextContiguous(const char* text, float* x, float* y, float startX, Color color, int lineNumber, Font font, bool wrapText, float maxWidth, float* maxX);
void DrawTextWithCyclingColors(const char* text, float* x, float* y, float startX, int* lineNumber, Font font, bool wrapText, float maxWidth, float* maxX);
void DrawLineNumber(int lineNumber, float* x, float y, Color color, Font font);
