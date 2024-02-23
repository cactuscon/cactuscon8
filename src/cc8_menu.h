using namespace Menu;

// TFT color table
const colorDef<uint16_t> colors[6] MEMMODE = {
  //{{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
  {{(uint16_t)Black, (uint16_t)Black}, {(uint16_t)Black, (uint16_t)Red,   (uint16_t)Red}}, //bgColor
  {{(uint16_t)White, (uint16_t)White},  {(uint16_t)White, (uint16_t)White, (uint16_t)White}},//fgColor
  {{(uint16_t)Red, (uint16_t)Red}, {(uint16_t)Yellow, (uint16_t)Yellow, (uint16_t)Yellow}}, //valColor
  {{(uint16_t)White, (uint16_t)White}, {(uint16_t)White, (uint16_t)White, (uint16_t)White}}, //unitColor
  {{(uint16_t)White, (uint16_t)Gray},  {(uint16_t)Black, (uint16_t)Red,  (uint16_t)White}}, //cursorColor
  {{(uint16_t)White, (uint16_t)Yellow}, {(uint16_t)Black,  (uint16_t)Red,   (uint16_t)Red}}, //titleColor
};

// menus
keyMap dPadBtn_map[]={
 {BTN_SEL, defaultNavCodes[enterCmd].ch, 35},
 {BTN_UP, defaultNavCodes[upCmd].ch, 35},
 {BTN_DOWN, defaultNavCodes[downCmd].ch, 35},
 {BTN_LEFT, defaultNavCodes[leftCmd].ch, 35},
 {BTN_RIGHT, defaultNavCodes[rightCmd].ch, 35},
};
keyIn<5> dPadBtns(dPadBtn_map);


//constMEM panel panels[] MEMMODE = {{0, 0, TFT_Width / fontW, TFT_Height / fontH}}; // Main menu panel
constMEM panel panels[] MEMMODE = {{0, 0, TFT_Width, TFT_Height / fontH}}; // Main menu panel
navNode* nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, sizeof(panels) / sizeof(panel)); //a list of panels and nodes
//idx_t tops[MAX_DEPTH]={0,0}; // store cursor positions for each level
idx_t eSpiTops[MAX_DEPTH] = {0};
TFT_eSPIOut eSpiOut(tft, colors, eSpiTops, pList, fontW, fontH + 1);
idx_t serialTops[MAX_DEPTH] = {0};
serialOut outSerial(Serial, serialTops);
menuOut *constMEM outputs[] MEMMODE = {&outSerial, &eSpiOut};            //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut *)); //outputs list
serialIn serial(Serial);
MENU_INPUTS(in, &dPadBtns, &serial);


const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit, hexDigit};
char buf1[]="0x11";//<-- menu will edit this text
result showData();
result gameOfLife();
result jigglometer();
result pulsometer();
result scanWifi();
result scanBLE();
void comingSoon();

// // max
// MENU(hrMenu, "Heart rate", doNothing, noEvent, wrapStyle,
//   OP("HRM", pulsometer, enterEvent),
//   OP("Settings", comingSoon, enterEvent),
//   EXIT("<Back")
// );

// // bluetooth
// MENU(btMenu, "Bluetooth", doNothing, noEvent, wrapStyle,
//   OP("Settings", scanBLE, enterEvent),
//   EXIT("<Back")
// );

// // wifi
// MENU(wifiMenu, "WiFi", doNothing, noEvent, wrapStyle,
//   OP("Settings", scanWifi, enterEvent),
//   EXIT("<Back")
// );

// mq-3
//MENU(gyroMenu, "Accel/Gyro", doNothing, noEvent, wrapStyle,
//  OP("Jigglometer", comingSoon, enterEvent),
//  EXIT("<Back")
//);

// settings
//MENU(settingsMenu, "Settings", doNoting, noEvent, wrapStyle,
//  FIELD("Name", userName),
//  FIELD("Data", userData),
//  FIELD("BT PIN", btPin)  
//);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Game of Life", gameOfLife, enterEvent)
  ,OP("Jigglometer", jigglometer, enterEvent)
  ,OP("Heart Rate", pulsometer, enterEvent)
  ,OP("Scan BLE", scanBLE, enterEvent)
  ,OP("Scan WiFi", scanWifi, enterEvent)
);
//  ,SUBMENU(wifiMenu)

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);