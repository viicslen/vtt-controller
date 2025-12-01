#ifndef IRREMOTE_H
#define IRREMOTE_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "Config.h"

class IRRemoteManager {
public:
    IRRemoteManager();
    
    // Initialize IR sender
    void begin();
    
    // Send LG TV command by key character
    // Returns the command name if sent, nullptr if no matching command
    const char* sendCommand(char key);
    
    // Send LG TV command by HID key code
    // Returns the command name if sent, nullptr if no matching command
    const char* sendCommandByHID(uint8_t hidKeyCode);
    
    // Send raw NEC code
    void sendNEC(uint32_t code);
    
private:
    IRsend irsend;
    
    // LG TV command structure
    struct LGCommand {
        char key;           // Keyboard character
        uint8_t hidKey;     // HID key code (alternative)
        uint32_t necCode;   // NEC code
        const char* name;   // Display name
    };
    
    // LG TV command table
    static const LGCommand commands[];
    static const int numCommands;
    
    // Find command by key
    const LGCommand* findCommandByKey(char key) const;
    
    // Find command by HID key code
    const LGCommand* findCommandByHID(uint8_t hidKeyCode) const;
};

#endif // IRREMOTE_H
