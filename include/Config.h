#ifndef CONFIG_H
#define CONFIG_H

// Pin Definitions
#define IR_LED_PIN 44
#define GO_BUTTON_PIN 0
#define DISPLAY_BACKLIGHT_PIN 38

// Display Configuration
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define HEADER_HEIGHT 20

// Timing Configuration
#define MODE_SWITCH_DEBOUNCE_MS 300
#define CONNECTION_CHECK_INTERVAL_MS 500
#define MAIN_LOOP_DELAY_MS 10

// Mouse Configuration
#define MOUSE_DEFAULT_SPEED 10
#define MOUSE_SHIFT_MULTIPLIER 2

// Audio Feedback
#define MODE_SWITCH_TONE_FREQ 1000
#define MODE_SWITCH_TONE_DURATION 100
#define IR_SEND_TONE_FREQ 800
#define IR_SEND_TONE_DURATION 30
#define STARTUP_TONE_FREQ_1 1000
#define STARTUP_TONE_FREQ_2 1500
#define STARTUP_TONE_DURATION 100

// HID Report Configuration
#define MAX_SIMULTANEOUS_KEYS 6

// BLE Configuration
#define BLE_DEVICE_NAME "M5Cardputer"

// Color Definitions (RGB565)
#define COLOR_BACKGROUND 0x0000   // Black
#define COLOR_TEXT 0xFFFF         // White
#define COLOR_HEADER 0x4208       // Dark Gray
#define COLOR_KEYBOARD 0x07E0     // Green
#define COLOR_MOUSE 0x001F        // Blue
#define COLOR_REMOTE 0xF800       // Red

// Connection Status Colors (RGB565)
#define COLOR_USB_CONNECTED 0x07E0   // Green (same as TFT_GREEN)
#define COLOR_BLE_CONNECTED 0x001F   // Blue (same as TFT_BLUE)
#define COLOR_DISCONNECTED 0xF800    // Red (same as TFT_RED)

// HID Key Codes Reference
#define HID_KEY_ARROW_UP 0x52
#define HID_KEY_ARROW_DOWN 0x51
#define HID_KEY_ARROW_LEFT 0x50
#define HID_KEY_ARROW_RIGHT 0x4F
#define HID_KEY_ENTER 0x28
#define HID_KEY_BACKSLASH 0x31
#define HID_KEY_TAB 0x2B
#define HID_KEY_BACKSPACE 0x2A
#define HID_KEY_SPACE 0x2C

// LG TV IR NEC Codes (32-bit)
#define LG_POWER 0x20DF10EF
#define LG_VOL_UP 0x20DF40BF
#define LG_VOL_DOWN 0x20DFC03F
#define LG_CH_UP 0x20DF00FF
#define LG_CH_DOWN 0x20DF807F
#define LG_MUTE 0x20DF906F
#define LG_MENU 0x20DFC23D
#define LG_OK 0x20DF22DD
#define LG_UP 0x20DF02FD
#define LG_DOWN 0x20DF827D
#define LG_LEFT 0x20DFE01F
#define LG_RIGHT 0x20DF609F
#define LG_BACK 0x20DF14EB
#define LG_HOME 0x20DF3EC1
#define LG_INPUT 0x20DFD02F
#define LG_NUM_0 0x20DF08F7
#define LG_NUM_1 0x20DF8877
#define LG_NUM_2 0x20DF48B7
#define LG_NUM_3 0x20DFC837
#define LG_NUM_4 0x20DF28D7
#define LG_NUM_5 0x20DFA857
#define LG_NUM_6 0x20DF6897
#define LG_NUM_7 0x20DFE817
#define LG_NUM_8 0x20DF18E7
#define LG_NUM_9 0x20DF9867

#endif // CONFIG_H
