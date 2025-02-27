#include "KeyboardController.h"
#include <cstdlib>
#include <iostream>
#include "Windows.h"

// WARNING! CLicks will only work with administrator rights
void openOnScreenKeyboard() {
    int result = system("osk.exe");
    if (result != 0) {
        std::cerr << "Error: could not open on-screen keyboard!" << std::endl;
    }
    std::cout << "Open OnScreen Keyboard" << std::endl;
}
// WARNING! CLicks will only work with administrator rights

bool setClipboardText(const std::wstring& text) {
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();

    size_t size = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hMem) {
        CloseClipboard();
        return false;
    }

    memcpy(GlobalLock(hMem), text.c_str(), size);
    GlobalUnlock(hMem);
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();

    return true;
}
