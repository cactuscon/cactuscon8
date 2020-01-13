#ifndef __constants_h__
#define __constants_h__

#include <TFT_eSPI.h>

//colors
#define Black RGB565(0,0,0)
#define Red RGB565(255,0,0)
#define Green RGB565(0,255,0)
#define Blue RGB565(0,0,255)
#define Gray RGB565(128,128,128)
#define LighterRed RGB565(255,150,150)
#define LighterGreen RGB565(150,255,150)
#define LighterBlue RGB565(150,150,255)
#define LighterGray RGB565(211,211,211)
#define DarkerRed RGB565(150,0,0)
#define DarkerGreen RGB565(0,150,0)
#define DarkerBlue RGB565(0,0,150)
#define Cyan RGB565(0,255,255)
#define Magenta RGB565(255,0,255)
#define Yellow RGB565(255,255,0)
#define White RGB565(255,255,255)
#define DarkerOrange RGB565(255,140,0)

//display size
#define TFT_Width 135
#define TFT_Height 240

//font size plus margins
#define fontW 8
#define fontH 16

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

// touch pads
#define BTN_UP    15 // up - TPAD 2
#define BTN_SEL   13 // enter - TPAD 3
#define BTN_DOWN  12 // down - TPAD 4
#define BTN_LEFT  2  // left - TPAD 5
#define BTN_RIGHT 27 // right - TPAD 7

#define TPAD_THRESH 35
#define TPAD_DEBOUNCE_MS 100

// jigglometer
#define GRAPH_WIDTH (TFT_WIDTH-20)
#define GRAPH_HEIGHT TFT_HEIGHT
#define GRAPH_SIZE (sizeof(uint16_t)*GRAPH_WIDTH*(GRAPH_HEIGHT+1))

// menu
#define MAX_DEPTH 3

// T-Display
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

// ESP32 buttons
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

#endif