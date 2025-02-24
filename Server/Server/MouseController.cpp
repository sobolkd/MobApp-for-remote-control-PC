#include "MouseController.h"
#include <windows.h>
#include <iostream>

// function for moving
void moveCursor(int x, int y) {
    SetCursorPos(x, y);
}

// function for clicks
void clickMouse(bool leftButton) {
    if (leftButton) {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        std::cout << "Left Click" << std::endl;
    }
    else {
        mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        std::cout << "Right Click" << std::endl;
    }
}

//function for scroll
void scrollMouse(int amount) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = amount;

    SendInput(1, &input, sizeof(INPUT));
    std::cout << "Scroll..." << std::endl;
}

