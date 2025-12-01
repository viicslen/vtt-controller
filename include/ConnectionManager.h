#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <Arduino.h>
#include "USBHIDManager.h"
#include "BLEHIDDevice.h"

// Connection type enumeration
enum class ConnectionType {
    NONE,
    USB,
    BLE
};

class ConnectionManager {
public:
    ConnectionManager(USBHIDManager& usbManager, BLEHIDManager& bleManager);
    
    // Initialize connection manager
    void begin();
    
    // Update connection state (call periodically)
    // Returns true if connection state changed
    bool update();
    
    // Get current active connection type
    ConnectionType getActiveConnection() const;
    
    // Check if any connection is available
    bool isConnected() const;
    
    // Get connection status string
    const char* getConnectionStatusString() const;
    
    // Get connection color
    uint16_t getConnectionColor() const;
    
    // Send keyboard report through active connection
    void sendKeyboardReport(uint8_t modifiers, uint8_t* keys, uint8_t numKeys);
    
    // Send mouse report through active connection
    void sendMouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel = 0);
    
    // Release all keys through active connection
    void releaseAllKeys();
    
    // Release all mouse buttons through active connection
    void releaseAllMouseButtons();
    
private:
    USBHIDManager& usb;
    BLEHIDManager& ble;
    ConnectionType activeConnection;
    ConnectionType lastConnection;
    bool bleWasStarted;
    
    // Update BLE state based on USB connection
    void updateBLEState();
};

#endif // CONNECTIONMANAGER_H
