#ifndef USBHIDMANAGER_H
#define USBHIDMANAGER_H

#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>

class USBHIDManager {
public:
    USBHIDManager();
    
    // Initialize USB HID
    void begin();
    
    // Check if USB is connected
    bool isConnected() const;
    
    // Send keyboard report
    void sendKeyboardReport(uint8_t modifiers, uint8_t* keys, uint8_t numKeys);
    
    // Send mouse report
    void sendMouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel = 0);
    
    // Release all keys
    void releaseAllKeys();
    
    // Release all mouse buttons
    void releaseAllMouseButtons();
    
    // Press/release specific mouse button
    void mousePress(uint8_t button);
    void mouseRelease(uint8_t button);
    
    // Move mouse
    void mouseMove(int8_t x, int8_t y, int8_t wheel = 0);
    
private:
    USBHIDKeyboard keyboard;
    USBHIDMouse mouse;
    bool initialized;
    uint8_t lastButtons;
};

#endif // USBHIDMANAGER_H
