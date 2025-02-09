#include "MouseController.h"
#include <windows.h>

// function for moving
void moveCursor(int x, int y) {
    SetCursorPos(x, y);
}

// function for clicks
void clickMouse(bool leftButton) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;

    if (leftButton) {
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
    }
    else {
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        SendInput(1, &input, sizeof(INPUT));
        input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}
