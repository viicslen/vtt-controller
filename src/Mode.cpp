#include "Mode.h"
#include "Config.h"

ModeManager::ModeManager() : currentMode(Mode::KEYBOARD) {
}

Mode ModeManager::getCurrentMode() const {
    return currentMode;
}

void ModeManager::nextMode() {
    switch (currentMode) {
        case Mode::KEYBOARD:
            currentMode = Mode::MOUSE;
            break;
        case Mode::MOUSE:
            currentMode = Mode::TV_REMOTE;
            break;
        case Mode::TV_REMOTE:
            currentMode = Mode::KEYBOARD;
            break;
    }
}

const char* ModeManager::getModeName() const {
    switch (currentMode) {
        case Mode::KEYBOARD:
            return "KEYBOARD";
        case Mode::MOUSE:
            return "MOUSE";
        case Mode::TV_REMOTE:
            return "TV REMOTE";
        default:
            return "UNKNOWN";
    }
}

uint16_t ModeManager::getModeColor() const {
    switch (currentMode) {
        case Mode::KEYBOARD:
            return COLOR_KEYBOARD;
        case Mode::MOUSE:
            return COLOR_MOUSE;
        case Mode::TV_REMOTE:
            return COLOR_REMOTE;
        default:
            return COLOR_TEXT;
    }
}
