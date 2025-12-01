#include <M5Cardputer.h>
#include "Config.h"
#include "Mode.h"
#include "BLEHIDDevice.h"
#include "USBHIDManager.h"
#include "ConnectionManager.h"
#include "IRRemote.h"
#include "DisplayManager.h"

// Global objects
ModeManager modeManager;
USBHIDManager usbHID;
BLEHIDManager bleHID;
ConnectionManager connectionManager(usbHID, bleHID);
IRRemoteManager irRemote;
DisplayManager display;

// Timing variables
unsigned long lastConnectionCheck = 0;
unsigned long lastModeSwitch = 0;
bool lastGoButtonState = true; // GO button is active low

// Keyboard state tracking
String lastKeysString;

// Function prototypes
void handleKeyboardMode();
void handleMouseMode();
void handleTVRemoteMode();
void handleModeSwitch();

void setup() {
    // Initialize M5Cardputer
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    
    // Initialize display
    display.begin(&M5Cardputer.Display);
    
    // Initialize IR remote
    irRemote.begin();
    
    // Initialize connection manager (handles USB and BLE)
    connectionManager.begin();
    
    // Configure GO button as input
    pinMode(GO_BUTTON_PIN, INPUT_PULLUP);
    
    // Initial display update
    display.update(modeManager, connectionManager);
    
    // Play startup tone
    M5Cardputer.Speaker.tone(1000, 100);
    delay(100);
    M5Cardputer.Speaker.tone(1500, 100);
}

void loop() {
    // Update M5Cardputer (reads keyboard, buttons)
    M5Cardputer.update();
    
    // Check connection state periodically
    unsigned long currentTime = millis();
    if (currentTime - lastConnectionCheck >= CONNECTION_CHECK_INTERVAL_MS) {
        lastConnectionCheck = currentTime;
        
        bool connectionChanged = connectionManager.update();
        if (connectionChanged) {
            display.forceRedraw();
            display.update(modeManager, connectionManager);
            
            // Audio feedback for connection change
            if (connectionManager.isConnected()) {
                M5Cardputer.Speaker.tone(1200, 50);
            } else {
                M5Cardputer.Speaker.tone(400, 100);
            }
        }
    }
    
    // Check GO button for mode switching
    handleModeSwitch();
    
    // Handle current mode
    switch (modeManager.getCurrentMode()) {
        case Mode::KEYBOARD:
            handleKeyboardMode();
            break;
        case Mode::MOUSE:
            handleMouseMode();
            break;
        case Mode::TV_REMOTE:
            handleTVRemoteMode();
            break;
    }
    
    // Update display
    display.update(modeManager, connectionManager);
    
    // Small delay
    delay(MAIN_LOOP_DELAY_MS);
}

void handleModeSwitch() {
    bool currentGoButtonState = digitalRead(GO_BUTTON_PIN);
    unsigned long currentTime = millis();
    
    // Detect falling edge (button press) with debounce
    if (!currentGoButtonState && lastGoButtonState) {
        if (currentTime - lastModeSwitch >= MODE_SWITCH_DEBOUNCE_MS) {
            lastModeSwitch = currentTime;
            
            // Switch mode
            modeManager.nextMode();
            
            // Audio feedback
            M5Cardputer.Speaker.tone(MODE_SWITCH_TONE_FREQ, MODE_SWITCH_TONE_DURATION);
            
            // Visual feedback
            display.forceRedraw();
            
            // Release any held keys when switching modes
            connectionManager.releaseAllKeys();
            connectionManager.releaseAllMouseButtons();
        }
    }
    
    lastGoButtonState = currentGoButtonState;
}

void handleKeyboardMode() {
    // Only process if connected
    if (!connectionManager.isConnected()) {
        return;
    }
    
    // Check for keyboard input
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();
        
        // Get typed characters for display
        String keysString;
        for (auto key : status.word) {
            keysString += key;
        }
        
        // Get HID key codes
        uint8_t keys[6] = {0};
        uint8_t numKeys = 0;
        
        for (auto hidKey : status.hid_keys) {
            if (numKeys < 6) {
                keys[numKeys++] = hidKey;
            }
        }
        
        // Get modifiers
        uint8_t modifiers = status.modifiers;
        
        // Send keyboard report if we have keys
        if (numKeys > 0 || modifiers != 0) {
            connectionManager.sendKeyboardReport(modifiers, keys, numKeys);
            
            // Update display with last keys
            if (keysString.length() > 0) {
                lastKeysString = keysString;
                display.showKeyboardMode(lastKeysString.c_str());
            }
        }
    } else if (M5Cardputer.Keyboard.isChange() && !M5Cardputer.Keyboard.isPressed()) {
        // Key released - send empty report
        connectionManager.releaseAllKeys();
    }
}

void handleMouseMode() {
    // Only process if connected
    if (!connectionManager.isConnected()) {
        return;
    }
    
    // Check for keyboard input
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();
        
        int8_t moveX = 0;
        int8_t moveY = 0;
        uint8_t buttons = 0;
        const char* direction = nullptr;
        
        // Calculate movement speed (double with shift)
        int speed = MOUSE_DEFAULT_SPEED;
        if (status.shift) {
            speed *= MOUSE_SHIFT_MULTIPLIER;
        }
        
        // Check for arrow keys and other inputs
        for (auto hidKey : status.hid_keys) {
            switch (hidKey) {
                case HID_KEY_ARROW_UP:
                    moveY = -speed;
                    direction = "UP";
                    break;
                case HID_KEY_ARROW_DOWN:
                    moveY = speed;
                    direction = "DOWN";
                    break;
                case HID_KEY_ARROW_LEFT:
                    moveX = -speed;
                    direction = "LEFT";
                    break;
                case HID_KEY_ARROW_RIGHT:
                    moveX = speed;
                    direction = "RIGHT";
                    break;
                case HID_KEY_ENTER:
                    buttons |= 0x01; // Left click
                    direction = "L-CLICK";
                    break;
                case HID_KEY_BACKSLASH:
                    buttons |= 0x02; // Right click
                    direction = "R-CLICK";
                    break;
            }
        }
        
        // Send mouse report if there's any action
        if (moveX != 0 || moveY != 0 || buttons != 0) {
            connectionManager.sendMouseReport(buttons, moveX, moveY);
            
            if (direction) {
                display.showMouseMode(direction);
            }
        }
    } else if (M5Cardputer.Keyboard.isChange() && !M5Cardputer.Keyboard.isPressed()) {
        // Release mouse buttons on key release
        connectionManager.releaseAllMouseButtons();
    }
}

void handleTVRemoteMode() {
    // TV Remote mode works without connection (IR is local)
    
    // Check for keyboard input
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();
        
        const char* commandName = nullptr;
        
        // First try character-based commands
        for (auto key : status.word) {
            commandName = irRemote.sendCommand(key);
            if (commandName) {
                break;
            }
        }
        
        // If no character match, try HID key codes (for arrow keys, enter, etc.)
        if (!commandName) {
            for (auto hidKey : status.hid_keys) {
                commandName = irRemote.sendCommandByHID(hidKey);
                if (commandName) {
                    break;
                }
            }
        }
        
        // Provide feedback if command was sent
        if (commandName) {
            // Audio feedback
            M5Cardputer.Speaker.tone(IR_SEND_TONE_FREQ, IR_SEND_TONE_DURATION);
            
            // Visual feedback
            display.showRemoteMode(commandName);
        }
    }
}
