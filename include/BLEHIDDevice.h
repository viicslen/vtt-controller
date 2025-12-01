#ifndef BLEHIDDEVICE_H
#define BLEHIDDEVICE_H

#include <Arduino.h>
#include <NimBLEDevice.h>

// Forward declarations
class NimBLEHIDDevice;
class NimBLECharacteristic;

class BLEHIDManager {
public:
    BLEHIDManager();
    
    // Initialize BLE HID
    void begin();
    
    // Stop BLE
    void stop();
    
    // Start advertising
    void startAdvertising();
    
    // Stop advertising
    void stopAdvertising();
    
    // Check if BLE is connected
    bool isConnected() const;
    
    // Check if BLE is advertising
    bool isAdvertising() const;
    
    // Send keyboard report
    // Report format: 1 byte modifier + 1 byte reserved + 6 bytes keys
    void sendKeyboardReport(uint8_t modifiers, uint8_t* keys, uint8_t numKeys);
    
    // Send mouse report
    // Report format: 1 byte buttons + 1 byte X + 1 byte Y + 1 byte wheel
    void sendMouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel = 0);
    
    // Release all keys
    void releaseAllKeys();
    
    // Release all mouse buttons
    void releaseAllMouseButtons();
    
private:
    NimBLEServer* pServer;
    NimBLEHIDDevice* hid;
    NimBLECharacteristic* inputKeyboard;
    NimBLECharacteristic* inputMouse;
    bool connected;
    bool advertising;
    
    // Static callback for connection events
    static void onConnect(NimBLEServer* pServer);
    static void onDisconnect(NimBLEServer* pServer);
    
    // Static instance pointer for callbacks
    static BLEHIDManager* instance;
    
    // Server callbacks class
    class ServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer) override;
        void onDisconnect(NimBLEServer* pServer) override;
    };
};

#endif // BLEHIDDEVICE_H
