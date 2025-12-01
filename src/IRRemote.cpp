#include "IRRemote.h"
#include "Config.h"

// LG TV command table
const IRRemoteManager::LGCommand IRRemoteManager::commands[] = {
    {'p', 0, LG_POWER, "Power"},
    {'P', 0, LG_POWER, "Power"},
    {'+', 0, LG_VOL_UP, "Vol Up"},
    {'=', 0, LG_VOL_UP, "Vol Up"},  // = key without shift is common for +
    {'-', 0, LG_VOL_DOWN, "Vol Down"},
    {']', 0, LG_CH_UP, "Ch Up"},
    {'[', 0, LG_CH_DOWN, "Ch Down"},
    {'m', 0, LG_MUTE, "Mute"},
    {'M', 0, LG_MUTE, "Mute"},
    {'n', 0, LG_MENU, "Menu"},
    {'N', 0, LG_MENU, "Menu"},
    {'\n', HID_KEY_ENTER, LG_OK, "OK"},
    {0, HID_KEY_ARROW_UP, LG_UP, "Up"},
    {0, HID_KEY_ARROW_DOWN, LG_DOWN, "Down"},
    {0, HID_KEY_ARROW_LEFT, LG_LEFT, "Left"},
    {0, HID_KEY_ARROW_RIGHT, LG_RIGHT, "Right"},
    {'b', 0, LG_BACK, "Back"},
    {'B', 0, LG_BACK, "Back"},
    {'h', 0, LG_HOME, "Home"},
    {'H', 0, LG_HOME, "Home"},
    {'i', 0, LG_INPUT, "Input"},
    {'I', 0, LG_INPUT, "Input"},
    {'0', 0x27, LG_NUM_0, "0"},
    {'1', 0x1E, LG_NUM_1, "1"},
    {'2', 0x1F, LG_NUM_2, "2"},
    {'3', 0x20, LG_NUM_3, "3"},
    {'4', 0x21, LG_NUM_4, "4"},
    {'5', 0x22, LG_NUM_5, "5"},
    {'6', 0x23, LG_NUM_6, "6"},
    {'7', 0x24, LG_NUM_7, "7"},
    {'8', 0x25, LG_NUM_8, "8"},
    {'9', 0x26, LG_NUM_9, "9"},
};

const int IRRemoteManager::numCommands = sizeof(commands) / sizeof(commands[0]);

IRRemoteManager::IRRemoteManager() : irsend(IR_LED_PIN) {
}

void IRRemoteManager::begin() {
    irsend.begin();
}

const char* IRRemoteManager::sendCommand(char key) {
    const LGCommand* cmd = findCommandByKey(key);
    if (cmd) {
        sendNEC(cmd->necCode);
        return cmd->name;
    }
    return nullptr;
}

const char* IRRemoteManager::sendCommandByHID(uint8_t hidKeyCode) {
    const LGCommand* cmd = findCommandByHID(hidKeyCode);
    if (cmd) {
        sendNEC(cmd->necCode);
        return cmd->name;
    }
    return nullptr;
}

void IRRemoteManager::sendNEC(uint32_t code) {
    irsend.sendNEC(code, 32);
}

const IRRemoteManager::LGCommand* IRRemoteManager::findCommandByKey(char key) const {
    for (int i = 0; i < numCommands; i++) {
        if (commands[i].key == key) {
            return &commands[i];
        }
    }
    return nullptr;
}

const IRRemoteManager::LGCommand* IRRemoteManager::findCommandByHID(uint8_t hidKeyCode) const {
    for (int i = 0; i < numCommands; i++) {
        if (commands[i].hidKey == hidKeyCode && hidKeyCode != 0) {
            return &commands[i];
        }
    }
    return nullptr;
}
