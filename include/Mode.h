#ifndef MODE_H
#define MODE_H

#include <Arduino.h>

// Mode enumeration
enum class Mode {
    KEYBOARD,
    MOUSE,
    TV_REMOTE
};

class ModeManager {
public:
    ModeManager();
    
    // Get current mode
    Mode getCurrentMode() const;
    
    // Cycle to next mode
    void nextMode();
    
    // Get mode name as string
    const char* getModeName() const;
    
    // Get mode color (RGB565)
    uint16_t getModeColor() const;
    
private:
    Mode currentMode;
};

#endif // MODE_H
