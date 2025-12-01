#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Arduino.h>
#include <M5GFX.h>
#include "Mode.h"
#include "ConnectionManager.h"
#include "Config.h"

class DisplayManager {
public:
    DisplayManager();
    
    // Initialize display
    void begin(M5GFX* display);
    
    // Update display with current mode and connection
    void update(const ModeManager& mode, const ConnectionManager& connection);
    
    // Show mode-specific content
    void showKeyboardMode(const char* lastKeys);
    void showMouseMode(const char* direction);
    void showRemoteMode(const char* lastCommand);
    
    // Show status message
    void showStatus(const char* message);
    
    // Show notification (temporary message)
    void showNotification(const char* message, uint16_t color = COLOR_TEXT);
    
    // Force full redraw
    void forceRedraw();
    
private:
    M5GFX* lcd;
    M5Canvas canvas;
    
    // Cached state for efficient updates
    Mode lastMode;
    ConnectionType lastConnection;
    String lastContent;
    String lastStatus;
    bool needsFullRedraw;
    
    // Draw header bar
    void drawHeader(const ModeManager& mode, const ConnectionManager& connection);
    
    // Draw main content area
    void drawContent(const char* content);
    
    // Draw connection indicator
    void drawConnectionIndicator(const ConnectionManager& connection);
    
    // Clear main area
    void clearMainArea();
    
    // Get instructions for current mode
    const char* getModeInstructions(Mode mode) const;
};

#endif // DISPLAYMANAGER_H
