#include "file_dialog_manager.h"
#include <string.h>

void InitFileDialogManager(FileDialogManager* manager) {
    manager->state = InitGuiWindowFileDialog(GetWorkingDirectory());
    manager->state.SelectFilePressed = false;
    manager->state.windowActive = true;
    strcpy(manager->filePath, "\0");
    manager->loadedText = NULL;
    manager->fileChanged = false;
}

void UpdateFileDialog(FileDialogManager* manager) {
    // Display the file dialog
    GuiWindowFileDialog(&manager->state);

    // Handle file dialog
    if (manager->state.SelectFilePressed) {
        // Clean up previous file if exists
        if (manager->loadedText) {
            UnloadFileText(manager->loadedText);
            manager->loadedText = NULL;
        }

        // Load new file
        TextCopy(manager->filePath, TextFormat("%s/%s", manager->state.dirPathText, manager->state.fileNameText));
        manager->loadedText = LoadFileText(manager->filePath);
        manager->state.SelectFilePressed = false;
        manager->state.windowActive = false;
        manager->fileChanged = true;
    }
}

bool IsFileSelected(FileDialogManager* manager) {
    if (manager->fileChanged) {
        manager->fileChanged = false;
        return true;
    }
    return false;
}

char* GetLoadedText(FileDialogManager* manager) {
    return manager->loadedText;
}

const char* GetCurrentFilePath(FileDialogManager* manager) {
    return manager->filePath;
}

void ShowFileDialog(FileDialogManager* manager) {
    manager->state.windowActive = true;
}

void UnloadFileDialogManager(FileDialogManager* manager) {
    if (manager->loadedText) {
        UnloadFileText(manager->loadedText);
        manager->loadedText = NULL;
    }
}
