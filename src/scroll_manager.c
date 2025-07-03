// src/scroll_manager.c
#include "scroll_manager.h"
#include "constants.h"
#include "gruvbox.h"

void InitScrollManager(ScrollManager* manager) {
    manager->scrollY = 0.0f;
    manager->scrollX = 0.0f;
    manager->maxScrollY = 0.0f;
    manager->maxScrollX = 0.0f;
    manager->contentHeight = 0.0f;
    manager->contentWidth = 0.0f;
    manager->draggingVertical = false;
    manager->draggingHorizontal = false;
    manager->dragOffsetVertical = 0.0f;
    manager->dragOffsetHorizontal = 0.0f;
}

void ResetScrollPosition(ScrollManager* manager) {
    manager->scrollY = 0.0f;
    manager->scrollX = 0.0f;
}

void HandleScrollInput(ScrollManager* manager, bool textWrapEnabled, float viewportWidth, float viewportHeight) {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        if (IsKeyDown(KEY_LEFT_SHIFT) && !textWrapEnabled) {
            // Horizontal scrolling when shift is held and wrapping is off
            manager->scrollX -= wheel * SCROLL_SPEED;
            if (manager->scrollX < 0) manager->scrollX = 0;
            if (manager->scrollX > manager->maxScrollX) manager->scrollX = manager->maxScrollX;
        } else {
            // Vertical scrolling
            manager->scrollY -= wheel * SCROLL_SPEED;
            if (manager->scrollY < 0) manager->scrollY = 0;
            if (manager->scrollY > manager->maxScrollY) manager->scrollY = manager->maxScrollY;
        }
    }
}

void UpdateScrollBounds(ScrollManager* manager, float contentHeight, float contentWidth, 
                       float viewportWidth, float viewportHeight, bool textWrapEnabled) {
    manager->contentHeight = contentHeight;
    manager->contentWidth = contentWidth;
    manager->maxScrollY = contentHeight > viewportHeight ? contentHeight - viewportHeight : 0;
    manager->maxScrollX = (!textWrapEnabled && contentWidth > viewportWidth) ? contentWidth - viewportWidth : 0;
    
    // Reset horizontal scroll when enabling wrap
    if (textWrapEnabled) {
        manager->scrollX = 0;
        manager->maxScrollX = 0;
    }
}

void DrawScrollbars(ScrollManager* manager, float viewportWidth, float viewportHeight, 
                   float toolbarHeight, float numberLineWidth, float scrollbarWidth) {
    
    bool showVerticalScrollbar = (manager->maxScrollY > 0);
    bool showHorizontalScrollbar = (manager->maxScrollX > 0);
    
    // Draw vertical scrollbar
    if (showVerticalScrollbar) {
        Rectangle scrollbarBg = { 
            WINDOW_WIDTH - scrollbarWidth, 
            toolbarHeight, 
            scrollbarWidth, 
            viewportHeight - (showHorizontalScrollbar ? scrollbarWidth : 0) 
        };
        DrawRectangleRec(scrollbarBg, Gruvbox[GRUVBOX_DARK2]);

        // Calculate scrollbar thumb size and position
        float thumbHeight = (viewportHeight / manager->contentHeight) * scrollbarBg.height;
        if (thumbHeight < 20) thumbHeight = 20; // Minimum thumb size

        float thumbY = toolbarHeight + (manager->scrollY / manager->maxScrollY) * (scrollbarBg.height - thumbHeight);

        Rectangle scrollbarThumb = { 
            WINDOW_WIDTH - scrollbarWidth + 2, 
            thumbY, 
            scrollbarWidth - 4, 
            thumbHeight 
        };
        DrawRectangleRec(scrollbarThumb, Gruvbox[GRUVBOX_BRIGHT_BLUE]);

        // Handle vertical scrollbar dragging
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, scrollbarThumb)) {
                manager->draggingVertical = true;
                manager->dragOffsetVertical = mousePos.y - thumbY;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            manager->draggingVertical = false;
        }

        if (manager->draggingVertical) {
            Vector2 mousePos = GetMousePosition();
            float newThumbY = mousePos.y - manager->dragOffsetVertical;

            // Constrain thumb position
            if (newThumbY < toolbarHeight) newThumbY = toolbarHeight;
            if (newThumbY > toolbarHeight + scrollbarBg.height - thumbHeight) {
                newThumbY = toolbarHeight + scrollbarBg.height - thumbHeight;
            }

            // Update scroll position based on thumb position
            float thumbProgress = (newThumbY - toolbarHeight) / (scrollbarBg.height - thumbHeight);
            manager->scrollY = thumbProgress * manager->maxScrollY;
        }
    }

    // Draw horizontal scrollbar
    if (showHorizontalScrollbar) {
        Rectangle hScrollbarBg = {
            numberLineWidth,
            WINDOW_HEIGHT - scrollbarWidth,
            viewportWidth,
            scrollbarWidth
        };
        DrawRectangleRec(hScrollbarBg, Gruvbox[GRUVBOX_DARK2]);

        float thumbWidth = (viewportWidth / manager->contentWidth) * hScrollbarBg.width;
        if (thumbWidth < 20) thumbWidth = 20; // Minimum thumb size

        float thumbX = hScrollbarBg.x + (manager->scrollX / manager->maxScrollX) * (hScrollbarBg.width - thumbWidth);

        Rectangle hScrollbarThumb = {
            thumbX,
            hScrollbarBg.y + 2,
            thumbWidth,
            hScrollbarBg.height - 4
        };
        DrawRectangleRec(hScrollbarThumb, Gruvbox[GRUVBOX_BRIGHT_BLUE]);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, hScrollbarThumb)) {
                manager->draggingHorizontal = true;
                manager->dragOffsetHorizontal = mousePos.x - thumbX;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            manager->draggingHorizontal = false;
        }

        if (manager->draggingHorizontal) {
            Vector2 mousePos = GetMousePosition();
            float newThumbX = mousePos.x - manager->dragOffsetHorizontal;

            // Constrain thumb within scrollbar background
            if (newThumbX < hScrollbarBg.x) newThumbX = hScrollbarBg.x;
            if (newThumbX > hScrollbarBg.x + hScrollbarBg.width - thumbWidth) {
                newThumbX = hScrollbarBg.x + hScrollbarBg.width - thumbWidth;
            }

            float thumbProgress = (newThumbX - hScrollbarBg.x) / (hScrollbarBg.width - thumbWidth);
            manager->scrollX = thumbProgress * manager->maxScrollX;
        }
    }
}

Vector2 GetScrollOffset(const ScrollManager* manager) {
    return (Vector2){ -manager->scrollX, -manager->scrollY };
}
