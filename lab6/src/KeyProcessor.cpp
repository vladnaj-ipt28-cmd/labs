#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(ProcessingMode::NORMAL), shouldExit(false) {}

void KeyProcessor::handleKey(int key) {
    if (key == -1) return;

    switch (key) {
        case 27: // ESC
        case 'q':
        case 'Q':
            shouldExit = true;
            break;
        case '0':
            currentMode = ProcessingMode::NORMAL;
            break;
        case '1':
            currentMode = ProcessingMode::INVERT;
            break;
        case '2':
            currentMode = ProcessingMode::BLUR;
            break;
        case '3':
            currentMode = ProcessingMode::CANNY;
            break;
        case '4':
            currentMode = ProcessingMode::GLITCH;
            break;
        default:
            break;
    }
}

ProcessingMode KeyProcessor::getMode() const {
    return currentMode;
}

bool KeyProcessor::isExitRequested() const {
    return shouldExit;
}