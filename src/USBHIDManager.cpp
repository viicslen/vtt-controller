#include "USBHIDManager.h"
#include "Config.h"

USBHIDManager::USBHIDManager() : initialized(false), lastButtons(0) {
}

void USBHIDManager::begin() {
    if (!initialized) {
        USB.begin();
        keyboard.begin();
        mouse.begin();
        initialized = true;
    }
}

bool USBHIDManager::isConnected() const {
    // Check if USB is connected and configured
    return USB;
}

void USBHIDManager::sendKeyboardReport(uint8_t modifiers, uint8_t* keys, uint8_t numKeys) {
    if (!isConnected()) return;
    
    // Build KeyReport structure
    KeyReport report;
    report.modifiers = modifiers;
    report.reserved = 0;
    memset(report.keys, 0, sizeof(report.keys));
    
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS && i < numKeys; i++) {
        report.keys[i] = keys[i];
    }
    
    keyboard.sendReport(&report);
}

void USBHIDManager::sendMouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel) {
    if (!isConnected()) return;
    
    // Handle button state changes
    uint8_t changedButtons = buttons ^ lastButtons;
    
    // Press new buttons
    for (int i = 0; i < 3; i++) {
        uint8_t buttonMask = 1 << i;
        if ((changedButtons & buttonMask) && (buttons & buttonMask)) {
            mouse.press(buttonMask);
        }
    }
    
    // Release old buttons
    for (int i = 0; i < 3; i++) {
        uint8_t buttonMask = 1 << i;
        if ((changedButtons & buttonMask) && !(buttons & buttonMask)) {
            mouse.release(buttonMask);
        }
    }
    
    lastButtons = buttons;
    
    // Send movement
    if (x != 0 || y != 0 || wheel != 0) {
        mouse.move(x, y, wheel);
    }
}

void USBHIDManager::releaseAllKeys() {
    if (!isConnected()) return;
    
    KeyReport report;
    memset(&report, 0, sizeof(report));
    keyboard.sendReport(&report);
}

void USBHIDManager::releaseAllMouseButtons() {
    if (!isConnected()) return;
    
    if (lastButtons != 0) {
        for (int i = 0; i < 3; i++) {
            uint8_t buttonMask = 1 << i;
            if (lastButtons & buttonMask) {
                mouse.release(buttonMask);
            }
        }
        lastButtons = 0;
    }
}

void USBHIDManager::mousePress(uint8_t button) {
    if (!isConnected()) return;
    mouse.press(button);
    lastButtons |= button;
}

void USBHIDManager::mouseRelease(uint8_t button) {
    if (!isConnected()) return;
    mouse.release(button);
    lastButtons &= ~button;
}

void USBHIDManager::mouseMove(int8_t x, int8_t y, int8_t wheel) {
    if (!isConnected()) return;
    mouse.move(x, y, wheel);
}
