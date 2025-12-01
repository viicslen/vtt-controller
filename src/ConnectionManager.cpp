#include "ConnectionManager.h"
#include "Config.h"

ConnectionManager::ConnectionManager(USBHIDManager& usbManager, BLEHIDManager& bleManager)
    : usb(usbManager), ble(bleManager), activeConnection(ConnectionType::NONE),
      lastConnection(ConnectionType::NONE), bleWasStarted(false) {
}

void ConnectionManager::begin() {
    // Initialize USB first (priority connection)
    usb.begin();
    
    // Initialize BLE (fallback connection)
    ble.begin();
    bleWasStarted = true;
    
    // Determine initial connection state
    update();
}

bool ConnectionManager::update() {
    lastConnection = activeConnection;
    
    // Check USB first (priority)
    if (usb.isConnected()) {
        activeConnection = ConnectionType::USB;
        
        // Stop BLE when USB is connected to save power
        if (bleWasStarted && ble.isAdvertising()) {
            ble.stopAdvertising();
        }
    }
    // Check BLE if USB not connected
    else if (ble.isConnected()) {
        activeConnection = ConnectionType::BLE;
    }
    // Neither connected
    else {
        activeConnection = ConnectionType::NONE;
        
        // Start BLE advertising if not already running
        if (!ble.isAdvertising()) {
            ble.startAdvertising();
        }
    }
    
    // Return true if connection state changed
    return activeConnection != lastConnection;
}

ConnectionType ConnectionManager::getActiveConnection() const {
    return activeConnection;
}

bool ConnectionManager::isConnected() const {
    return activeConnection != ConnectionType::NONE;
}

const char* ConnectionManager::getConnectionStatusString() const {
    switch (activeConnection) {
        case ConnectionType::USB:
            return "USB Connected";
        case ConnectionType::BLE:
            return "BLE Connected";
        case ConnectionType::NONE:
            if (ble.isAdvertising()) {
                return "Waiting for connection...";
            }
            return "Disconnected";
        default:
            return "Unknown";
    }
}

uint16_t ConnectionManager::getConnectionColor() const {
    switch (activeConnection) {
        case ConnectionType::USB:
            return TFT_GREEN;
        case ConnectionType::BLE:
            return TFT_BLUE;
        case ConnectionType::NONE:
        default:
            return TFT_RED;
    }
}

void ConnectionManager::sendKeyboardReport(uint8_t modifiers, uint8_t* keys, uint8_t numKeys) {
    switch (activeConnection) {
        case ConnectionType::USB:
            usb.sendKeyboardReport(modifiers, keys, numKeys);
            break;
        case ConnectionType::BLE:
            ble.sendKeyboardReport(modifiers, keys, numKeys);
            break;
        default:
            break;
    }
}

void ConnectionManager::sendMouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel) {
    switch (activeConnection) {
        case ConnectionType::USB:
            usb.sendMouseReport(buttons, x, y, wheel);
            break;
        case ConnectionType::BLE:
            ble.sendMouseReport(buttons, x, y, wheel);
            break;
        default:
            break;
    }
}

void ConnectionManager::releaseAllKeys() {
    switch (activeConnection) {
        case ConnectionType::USB:
            usb.releaseAllKeys();
            break;
        case ConnectionType::BLE:
            ble.releaseAllKeys();
            break;
        default:
            break;
    }
}

void ConnectionManager::releaseAllMouseButtons() {
    switch (activeConnection) {
        case ConnectionType::USB:
            usb.releaseAllMouseButtons();
            break;
        case ConnectionType::BLE:
            ble.releaseAllMouseButtons();
            break;
        default:
            break;
    }
}

void ConnectionManager::updateBLEState() {
    if (usb.isConnected()) {
        // Stop BLE when USB is connected
        if (ble.isAdvertising()) {
            ble.stopAdvertising();
        }
    } else {
        // Start BLE when USB is disconnected
        if (!ble.isConnected() && !ble.isAdvertising()) {
            ble.startAdvertising();
        }
    }
}
