#include "KeyboardController.h"
#include <cstdlib>
#include <iostream>

void openOnScreenKeyboard() {
    int result = system("osk.exe");
    if (result != 0) {
        std::cerr << "Error: could not open on-screen keyboard!" << std::endl;
    }
    std::cout << "Open OnScreen Keyboard" << std::endl;
}
// WARNING! CLicks will only work with administrator rights
// WARNING! CLicks will only work with administrator rights
// WARNING! CLicks will only work with administrator rights