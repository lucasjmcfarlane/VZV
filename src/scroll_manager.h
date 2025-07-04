#pragma once

#include <raylib.h>
#include <stdbool.h>

typedef struct {
    float scrollY;
    float scrollX;
    float maxScrollY;
    float maxScrollX;
    float contentHeight;
    float contentWidth;
    
    // Scrollbar dragging state
    bool draggingVertical;
    bool draggingHorizontal;
    float dragOffsetVertical;
    float dragOffsetHorizontal;
} ScrollManager;

// Initialize the scroll manager
void InitScrollManager(ScrollManager* manager);

// Reset scroll position (useful when loading new files)
void ResetScrollPosition(ScrollManager* manager);

// Handle scroll input (mouse wheel)
void HandleScrollInput(ScrollManager* manager, bool textWrapEnabled, float viewportWidth, float viewportHeight);

// Update scroll bounds based on content dimensions
void UpdateScrollBounds(ScrollManager* manager, float contentHeight, float contentWidth, 
                       float viewportWidth, float viewportHeight, bool textWrapEnabled);

// Draw scrollbars and handle dragging
void DrawScrollbars(ScrollManager* manager, float viewportWidth, float viewportHeight, 
                   float toolbarHeight, float numberLineWidth, float scrollbarWidth);

// Get current scroll offsets
Vector2 GetScrollOffset(const ScrollManager* manager);
