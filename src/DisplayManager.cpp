#include "DisplayManager.h"

DisplayManager::DisplayManager() 
    : lcd(nullptr), lastMode(Mode::KEYBOARD), lastConnection(ConnectionType::NONE),
      needsFullRedraw(true) {
}

void DisplayManager::begin(M5GFX* display) {
    lcd = display;
    
    // Configure display
    lcd->setRotation(1);
    lcd->fillScreen(COLOR_BACKGROUND);
    
    // Create canvas for flicker-free drawing
    canvas.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    canvas.setTextDatum(MC_DATUM);
    
    needsFullRedraw = true;
}

void DisplayManager::update(const ModeManager& mode, const ConnectionManager& connection) {
    if (!lcd) return;
    
    // Check if we need to redraw
    bool modeChanged = mode.getCurrentMode() != lastMode;
    bool connectionChanged = connection.getActiveConnection() != lastConnection;
    
    if (!needsFullRedraw && !modeChanged && !connectionChanged) {
        return;
    }
    
    // Clear canvas
    canvas.fillSprite(COLOR_BACKGROUND);
    
    // Draw header
    drawHeader(mode, connection);
    
    // Draw mode-specific content
    canvas.setTextColor(COLOR_TEXT);
    canvas.setTextSize(1);
    
    int yPos = HEADER_HEIGHT + 20;
    
    // Connection status
    canvas.setTextDatum(MC_DATUM);
    canvas.setTextColor(connection.getConnectionColor());
    canvas.drawString(connection.getConnectionStatusString(), SCREEN_WIDTH / 2, yPos);
    
    // Mode instructions
    yPos += 25;
    canvas.setTextColor(COLOR_TEXT);
    canvas.setTextSize(1);
    
    const char* instructions = getModeInstructions(mode.getCurrentMode());
    if (instructions) {
        canvas.setTextDatum(TL_DATUM);
        canvas.drawString(instructions, 10, yPos);
    }
    
    // Draw last content if available
    if (lastContent.length() > 0) {
        yPos += 25;
        canvas.setTextDatum(MC_DATUM);
        canvas.setTextColor(mode.getModeColor());
        canvas.setTextSize(2);
        canvas.drawString(lastContent.c_str(), SCREEN_WIDTH / 2, yPos + 15);
    }
    
    // Push canvas to display
    canvas.pushSprite(0, 0);
    
    // Update cached state
    lastMode = mode.getCurrentMode();
    lastConnection = connection.getActiveConnection();
    needsFullRedraw = false;
}

void DisplayManager::showKeyboardMode(const char* lastKeys) {
    if (lastKeys && strlen(lastKeys) > 0) {
        lastContent = lastKeys;
        needsFullRedraw = true;
    }
}

void DisplayManager::showMouseMode(const char* direction) {
    if (direction && strlen(direction) > 0) {
        lastContent = direction;
        needsFullRedraw = true;
    }
}

void DisplayManager::showRemoteMode(const char* lastCommand) {
    if (lastCommand && strlen(lastCommand) > 0) {
        lastContent = lastCommand;
        needsFullRedraw = true;
    }
}

void DisplayManager::showStatus(const char* message) {
    lastStatus = message;
    needsFullRedraw = true;
}

void DisplayManager::showNotification(const char* message, uint16_t color) {
    if (!lcd) return;
    
    // Draw notification at bottom of screen
    canvas.fillRect(0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, 20, COLOR_HEADER);
    canvas.setTextColor(color);
    canvas.setTextDatum(MC_DATUM);
    canvas.setTextSize(1);
    canvas.drawString(message, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 10);
    canvas.pushSprite(0, 0);
}

void DisplayManager::forceRedraw() {
    needsFullRedraw = true;
    lastContent = "";
}

void DisplayManager::drawHeader(const ModeManager& mode, const ConnectionManager& connection) {
    // Draw header background
    canvas.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, COLOR_HEADER);
    
    // Draw mode name with color
    canvas.setTextColor(mode.getModeColor());
    canvas.setTextDatum(ML_DATUM);
    canvas.setTextSize(1);
    canvas.drawString(mode.getModeName(), 5, HEADER_HEIGHT / 2);
    
    // Draw connection indicator
    drawConnectionIndicator(connection);
}

void DisplayManager::drawConnectionIndicator(const ConnectionManager& connection) {
    // Draw connection status indicator on right side
    uint16_t indicatorColor = connection.getConnectionColor();
    
    // Draw filled circle as indicator
    int indicatorX = SCREEN_WIDTH - 15;
    int indicatorY = HEADER_HEIGHT / 2;
    canvas.fillCircle(indicatorX, indicatorY, 6, indicatorColor);
    
    // Draw connection type abbreviation
    const char* connType = "";
    switch (connection.getActiveConnection()) {
        case ConnectionType::USB:
            connType = "U";
            break;
        case ConnectionType::BLE:
            connType = "B";
            break;
        default:
            connType = "X";
            break;
    }
    
    canvas.setTextColor(COLOR_TEXT);
    canvas.setTextDatum(MR_DATUM);
    canvas.drawString(connType, SCREEN_WIDTH - 25, HEADER_HEIGHT / 2);
}

void DisplayManager::clearMainArea() {
    canvas.fillRect(0, HEADER_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - HEADER_HEIGHT, COLOR_BACKGROUND);
}

const char* DisplayManager::getModeInstructions(Mode mode) const {
    switch (mode) {
        case Mode::KEYBOARD:
            return "Type to send keystrokes";
        case Mode::MOUSE:
            return "Arrows:Move Enter:L-Click Bkslash:R-Click";
        case Mode::TV_REMOTE:
            return "P:Pwr +/-:Vol [/]:Ch M:Mute";
        default:
            return "";
    }
}
