#pragma once

#include "gui_window_file_dialog.h"

typedef struct {
    GuiWindowFileDialogState state;
    char filePath[512];
    char* loadedText;
    bool fileChanged;
} FileDialogManager;

// Initialize the file dialog manager
void InitFileDialogManager(FileDialogManager* manager);

// Update the file dialog (handles input and drawing)
void UpdateFileDialog(FileDialogManager* manager);

// Check if a file was selected and get the loaded text
bool IsFileSelected(FileDialogManager* manager);

// Get the loaded text (returns NULL if no file loaded)
char* GetLoadedText(FileDialogManager* manager);

// Get the current file path
const char* GetCurrentFilePath(FileDialogManager* manager);

// Show the file dialog
void ShowFileDialog(FileDialogManager* manager);

// Clean up resources
void UnloadFileDialogManager(FileDialogManager* manager);
