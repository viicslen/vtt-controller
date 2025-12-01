#include "BLEHIDDevice.h"
#include "Config.h"
#include <NimBLEHIDDevice.h>

// Static instance pointer for callbacks
BLEHIDManager* BLEHIDManager::instance = nullptr;

// Combined HID Report Descriptor for Keyboard (Report ID 1) and Mouse (Report ID 2)
static const uint8_t hidReportDescriptor[] = {
    // Keyboard Report Descriptor (Report ID 1)
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x05, 0x07,        //   Usage Page (Key Codes)
    0x19, 0xE0,        //   Usage Minimum (224)
    0x29, 0xE7,        //   Usage Maximum (231)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data, Variable, Absolute) - Modifier byte
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x03,        //   Input (Constant) - Reserved byte
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x65,        //   Logical Maximum (101)
    0x05, 0x07,        //   Usage Page (Key Codes)
    0x19, 0x00,        //   Usage Minimum (0)
    0x29, 0x65,        //   Usage Maximum (101)
    0x81, 0x00,        //   Input (Data, Array) - Key array
    0xC0,              // End Collection

    // Mouse Report Descriptor (Report ID 2)
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x02,        //     Report ID (2)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (1)
    0x29, 0x03,        //     Usage Maximum (3)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x03,        //     Report Count (3)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data, Variable, Absolute) - Buttons
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x05,        //     Report Size (5)
    0x81, 0x03,        //     Input (Constant) - Padding
    0x05, 0x01,        //     Usage Page (Generic Desktop)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data, Variable, Relative)
    0xC0,              //   End Collection
    0xC0               // End Collection
};

// Server callbacks implementation
void BLEHIDManager::ServerCallbacks::onConnect(NimBLEServer* pServer) {
    if (BLEHIDManager::instance) {
        BLEHIDManager::instance->connected = true;
        BLEHIDManager::instance->advertising = false;
    }
}

void BLEHIDManager::ServerCallbacks::onDisconnect(NimBLEServer* pServer) {
    if (BLEHIDManager::instance) {
        BLEHIDManager::instance->connected = false;
        // Restart advertising on disconnect
        BLEHIDManager::instance->startAdvertising();
    }
}

BLEHIDManager::BLEHIDManager() 
    : pServer(nullptr), hid(nullptr), inputKeyboard(nullptr), inputMouse(nullptr),
      connected(false), advertising(false) {
    instance = this;
}

void BLEHIDManager::begin() {
    NimBLEDevice::init(BLE_DEVICE_NAME);
    
    // Set power level
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    
    // Create server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    
    // Create HID device
    hid = new NimBLEHIDDevice(pServer);
    
    // Set manufacturer and PnP info
    hid->manufacturer()->setValue("M5Stack");
    // Using USB Implementers Forum (USB-IF) generic test vendor ID
    // For production, obtain a proper vendor ID from USB-IF
    hid->pnp(0x02, 0x1915, 0xEEEE, 0x0001); // Generic HID device
    hid->hidInfo(0x00, 0x01); // Country: not localized, Flags: remote wake
    
    // Set report map
    hid->reportMap((uint8_t*)hidReportDescriptor, sizeof(hidReportDescriptor));
    
    // Create input report characteristics
    inputKeyboard = hid->inputReport(1); // Report ID 1 for keyboard
    inputMouse = hid->inputReport(2);    // Report ID 2 for mouse
    
    // Start HID service
    hid->startServices();
    
    // Start advertising
    startAdvertising();
}

void BLEHIDManager::stop() {
    stopAdvertising();
    if (pServer) {
        NimBLEDevice::deinit(true);
        pServer = nullptr;
        hid = nullptr;
        inputKeyboard = nullptr;
        inputMouse = nullptr;
    }
    connected = false;
    advertising = false;
}

void BLEHIDManager::startAdvertising() {
    if (!advertising && pServer) {
        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->setAppearance(HID_KEYBOARD);
        pAdvertising->addServiceUUID(hid->hidService()->getUUID());
        pAdvertising->start();
        advertising = true;
    }
}

void BLEHIDManager::stopAdvertising() {
    if (advertising) {
        NimBLEDevice::getAdvertising()->stop();
        advertising = false;
    }
}

bool BLEHIDManager::isConnected() const {
    return connected;
}

bool BLEHIDManager::isAdvertising() const {
    return advertising;
}

void BLEHIDManager::sendKeyboardReport(uint8_t modifiers, uint8_t* keys, uint8_t numKeys) {
    if (!connected || !inputKeyboard) return;
    
    // Build keyboard report: modifier + reserved + 6 keys
    uint8_t report[8] = {0};
    report[0] = modifiers;
    report[1] = 0; // Reserved
    
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS && i < numKeys; i++) {
        report[2 + i] = keys[i];
    }
    
    inputKeyboard->setValue(report, sizeof(report));
    inputKeyboard->notify();
}

void BLEHIDManager::sendMouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel) {
    if (!connected || !inputMouse) return;
    
    // Build mouse report: buttons + X + Y + wheel
    uint8_t report[4];
    report[0] = buttons;
    report[1] = (uint8_t)x;
    report[2] = (uint8_t)y;
    report[3] = (uint8_t)wheel;
    
    inputMouse->setValue(report, sizeof(report));
    inputMouse->notify();
}

void BLEHIDManager::releaseAllKeys() {
    if (!connected || !inputKeyboard) return;
    
    uint8_t report[8] = {0};
    inputKeyboard->setValue(report, sizeof(report));
    inputKeyboard->notify();
}

void BLEHIDManager::releaseAllMouseButtons() {
    if (!connected || !inputMouse) return;
    
    uint8_t report[4] = {0};
    inputMouse->setValue(report, sizeof(report));
    inputMouse->notify();
}
