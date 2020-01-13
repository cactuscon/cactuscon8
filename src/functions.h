#include "life.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

int isTouched(int pin) {
  int val = touchRead(pin);
  return val ? val < TPAD_THRESH: 0;
}

void button_init()
{
    btn1.setLongClickHandler([](Button2 & b) {
        btnClick = false;
        int r = digitalRead(TFT_BL);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setRotation(3);
        // TODO: decrease font or add newline
        tft.drawString("Sleeping. Press again to wake up.",  tft.width() / 2, tft.height() / 2 );
        espDelay(6000);
        digitalWrite(TFT_BL, !r);

        tft.writecommand(TFT_DISPOFF);
        tft.writecommand(TFT_SLPIN);
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start();
    });
    btn1.setPressedHandler([](Button2 & b) {
        Serial.println("Escaping...");
        btnClick = true;
    });

    // btn2.setPressedHandler([](Button2 & b) {
    //     btnClick = false;
    //     Serial.println("btn press wifi scan");
    // });
}

void button_loop()
{
    btn1.loop();
    btn2.loop();
}


void comingSoon() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setRotation(3);
  tft.drawString("Coming soon!",  tft.width() / 2, tft.height() / 2);
  delay(3000);
}

bool initMax() {
  Serial.println("Initializing MAX...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    return false;
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  return true;
}

bool initMpu() {
    // MPU
    while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Adafruit MPU6050 test!");

    // Try to initialize!
    if (!mpu.begin()) {
      Serial.println("Failed to find MPU6050 chip");
      // while (1) {
      //   delay(10);
      // }
      return false;
    }
    Serial.println("MPU6050 Found!");
    
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
    }
    
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
    }
  
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
    }
  
    Serial.println("");
    delay(100);
    return true;
}

void readMax() {
  //long irValue = particleSensor.getIR();
  irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

//  Serial.print("IR=");
//  Serial.print(irValue);
//  Serial.print(", BPM=");
//  Serial.print(beatsPerMinute);
//  Serial.print(", Avg BPM=");
//  Serial.print(beatAvg);
//
//  if (irValue < 50000)
//    Serial.print(" No finger?");
//
//  Serial.println();
}

void readMpu() {
  /* Take a new reading */
  mpu.read();

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" deg/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println("");
  //delay(500);
}

result showData()
{
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 500) {
        timeStamp = millis();

        // show touchpad data
        String tpad2 = "pad 2: Up " + String(touchRead(BTN_UP));
        String tpad3 = "pad 3: Ent " + String(touchRead(BTN_SEL));
        String tpad4 = "pad 4: Dwn " + String(touchRead(BTN_DOWN));
        String tpad5 = "pad 5: Lft " + String(touchRead(BTN_LEFT));
        String tpad7 = "pad 7: Rgt " + String(touchRead(BTN_RIGHT));

        //Serial.println(voltage2);
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);

        int tftHeight = tft.height();
        int rowHeight = 10;

        tft.drawString(tpad5,  tft.width() / 2, tftHeight - (rowHeight * 1));
        tft.drawString(tpad4,  tft.width() / 2, tftHeight - (rowHeight * 2));
        tft.drawString(tpad3,  tft.width() / 2, tftHeight - (rowHeight * 3));
        tft.drawString(tpad2,  tft.width() / 2, tftHeight - (rowHeight * 4));
        tft.drawString(tpad7,  tft.width() / 2, tftHeight - (rowHeight * 5));

        // show accelerometer data
        //readMpu();
        /* Take a new reading */
        mpu.read();

        /* Get new sensor events with the readings */
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        tft.drawString("X: " + String(a.acceleration.x) + " m/s^2, " + String(g.gyro.x) + " deg/s", 2, tftHeight - (rowHeight * 9));
        tft.drawString("Y: " + String(a.acceleration.y) + " m/s^2,  " + String(g.gyro.y) + " deg/s", 2, tftHeight - (rowHeight * 8));
        tft.drawString("Z: " + String(a.acceleration.z) + " m/s^2, " + String(g.gyro.z) + " deg/s", 2, tftHeight - (rowHeight * 7));

        // temp
        tft.drawString("temp: " + String(temp.temperature) + " C",  tft.width() / 2, tftHeight - (rowHeight * 10));

        // show heartrate data
        readMax();  
        tft.drawString("IR: " + String(irValue) + " C",  tft.width() / 2, tftHeight - (rowHeight * 13));
        tft.drawString("BPM: " + String(beatsPerMinute) + " C",  tft.width() / 2, tftHeight - (rowHeight * 12));
        tft.drawString("avgBPM: " + String(beatAvg) + " C",  tft.width() / 2, tftHeight - (rowHeight * 11));

        if (irValue < 50000)
            tft.drawString("No finger?",  tft.width() / 2, tftHeight - (rowHeight * 14));
        delay(500);
        return proceed;
    }
}

void addDataPoints(float v1, float v2, uint16_t color, float scale=1.0)
{
  v1 *= scale;
  v2 *= scale;
  int offset1 = (GRAPH_WIDTH-1)/2+int(v1);
  int offset2 = (GRAPH_WIDTH-1)/2+int(v2);

  if (offset1 < 0) {
    offset1 = 0;
  }
  if (offset1 >= GRAPH_WIDTH) {
    offset1 = GRAPH_WIDTH - 1;
  }
  if (offset2 < 0) {
    offset2 = 0;
  }
  if (offset2 >= GRAPH_WIDTH) {
    offset2 = GRAPH_WIDTH - 1;
  }

  uint16_t colorByteSwap = (color>>8) | (color<<8);

  // if (!(offset1 == 0 && offset2 == 0) && !(offset1 == GRAPH_WIDTH-1 && offset2 == GRAPH_WIDTH-1)) {
    for (int off = min(offset1, offset2); off <= max(offset1, offset2); off++) {
      data[(GRAPH_WIDTH*(GRAPH_HEIGHT-1))+off] = colorByteSwap;
    }
  // }
}

void addHRMDataPoints(float v1, float v2, uint16_t color, float scale=1.0)
{
  v1 *= scale;
  v2 *= scale;
  int offset1 = int(v1);
  int offset2 = int(v2);

  if (offset1 < 0) {
    offset1 = 0;
  }
  if (offset1 >= GRAPH_WIDTH) {
    offset1 = GRAPH_WIDTH - 1;
  }
  if (offset2 < 0) {
    offset2 = 0;
  }
  if (offset2 >= GRAPH_WIDTH) {
    offset2 = GRAPH_WIDTH - 1;
  }

  // uint16_t colorByteSwap = (color>>8) | (color<<8);

  // if (!(offset1 == 0 && offset2 == 0) && !(offset1 == GRAPH_WIDTH-1 && offset2 == GRAPH_WIDTH-1)) {
    for (int off = min(offset1, offset2); off <= max(offset1, offset2); off++) {
      // data[(GRAPH_WIDTH*(GRAPH_HEIGHT-1))+off] = colorByteSwap;
      data[(GRAPH_WIDTH*(GRAPH_HEIGHT-1))+off] = color;
    }
  // }
}

void resetTFT() {
  if (data != NULL) {
    free(data);
    data = NULL;
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextPadding(0);
  tft.setTextWrap(false);
  nav.refresh();
}

void jiggInit() {
  data = (uint16_t*)malloc(GRAPH_SIZE);

  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSerifItalic24pt7b);
  tft.drawString("Jigglometer", tft.width() / 2, tft.height() / 2);

  // delay for dramatic effect
  delay(3000);

  tft.setTextDatum(TL_DATUM);
  tft.setRotation(3);
  // tft.setTextFont(2);
  // tft.setTextSize(1);
  tft.setFreeFont(&FreeSerif12pt7b);
  bounce_offset = tft.textWidth(bounce_title);
  tft.setTextColor(TFT_PINK);
  tft.drawString(bounce_title, 0, 0);
  tft.setTextColor(TFT_DARKGREY);
  bounce_offset += 5;
  tft.drawString(":", bounce_offset, 0);
  bounce_offset += 10;
  // tft.setTextFont(1);
  // tft.setTextSize(2);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setFreeFont(&FreeMono12pt7b);
  tft.drawNumber(0, bounce_offset, 1);
  tft.setRotation(2);

  tft.readRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT+1, data);
  bounce = 0;

  // make sure last row is blanked out (right off screen)
  // memset(data+(GRAPH_WIDTH*GRAPH_HEIGHT), 0, sizeof(uint16_t)*GRAPH_WIDTH);

}

void jiggLoop() {
  /* Take a new reading */
  mpu.read();

  /* Get new sensor events with the readings */
  //sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // remove first row (left)
  memmove(data, data+GRAPH_WIDTH, sizeof(uint16_t)*GRAPH_WIDTH*GRAPH_HEIGHT);
  // add data (right)
  addDataPoints(last_x_val, g.gyro.x, TFT_RED, 0.5);
  addDataPoints(last_y_val, g.gyro.y, TFT_GREEN, 0.5);
  addDataPoints(last_z_val, g.gyro.z, TFT_BLUE, 0.5);
  // send data to the screen
  tft.pushRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, data);

  float tot_val = fabs(g.gyro.x) + fabs(g.gyro.y) + fabs(g.gyro.z);

  if (last_tot_min > tot_val) {
    last_tot_min = tot_val;
  }
  // detect bounce
  if (last_tot_val > max_threshold && tot_val < max_threshold && last_tot_min < min_threshold) {
    bounce++;
    tft.setRotation(3);
    tft.drawString(String(bounce)+" ", bounce_offset, 1);
    tft.setRotation(2);
    last_tot_min = tot_val;
  }
  last_tot_val = tot_val;

  // save previous values
  last_x_val = g.gyro.x;
  last_y_val = g.gyro.y;
  last_z_val = g.gyro.z;

  // Serial.print(g.gyro.z);
  // Serial.print(" ");
  // Serial.println(bounce);

  // Delay so we don't swamp the serial port
  // delay(5);
  //Serial.write(10);
}

result jigglometer()
{
  if (!mpuDetected) {
    char* msg = "No MPU sensor detected. Aborting.";
    Serial.println(msg);
    tft.fillScreen(TFT_BLACK);
    tft.drawString(msg, tft.width() / 2, tft.height() / 2);
    delay(3000);
    resetTFT();
    return proceed;
  }

  jiggInit();
  while(!btnClick) {
    jiggLoop();
    button_loop();
  }
  // reset button
  btnClick = false;
  Serial.println("leaving jigglometer...");
  // reset
  resetTFT();
  return proceed;
}

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

void bleInit() {
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  // pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.drawString("Scanning BLE...", tft.width() / 2, tft.height() / 2);
  tft.setTextFont(1);
}

void bleLoop() {
  tft.setTextDatum(TL_DATUM);
  tft.setCursor(0, 0);
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  int n = foundDevices.getCount();
  Serial.printf("\n===+ Found %d BLE +===\n", n);
  for (int i=0; i<n; i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    sprintf(buff,
            "%3d %s %s",
            device.getRSSI(),
            device.getAddress().toString().c_str(),
            device.getName().c_str());
    Serial.println(buff);
    if (i<18) {
      tft.println(String(buff) + "                              ");
    }
  }
  for (int i = n; i<18; i++) {
    tft.println("                                    ");
  }
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
}

void scanInit() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.drawString("Scanning Networks...", tft.width() / 2, tft.height() / 2);
  tft.setTextFont(1);
}

void scanLoop() {
  int16_t n = WiFi.scanComplete();
  if (n == -2) {
    WiFi.scanNetworks(true, true);
  }
  if (n < 0) {
    return;
  }

  tft.setTextDatum(TL_DATUM);
  tft.setCursor(0, 0);
  Serial.printf("\n===+ Found %d net +===\n", n);
  for (int i = 0; i < n; i++) {
    uint8_t *bssid = WiFi.BSSID(i);
    sprintf(buff,
            "%3d %02X%02X%02X%02X%02X%02X %s",
            WiFi.RSSI(i),
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
            WiFi.SSID(i).c_str());
    Serial.println(buff);
    if (i<18) {
      tft.println(String(buff) + "                              ");
    }
  }
  for (int i = n; i<18; i++) {
    tft.println("                                    ");
  }
  WiFi.scanNetworks(true, true);
}

void pulseInit() {
  data = (uint16_t*)malloc(GRAPH_SIZE);

  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSerifItalic24pt7b);
  tft.drawString("Pulsometer", tft.width() / 2, tft.height() / 2);

  // delay for dramatic effect
  delay(3000);

  tft.setTextDatum(TL_DATUM);
  tft.setRotation(3);
  // tft.setTextFont(2);
  // tft.setTextSize(1);
  tft.setFreeFont(&FreeSerif12pt7b);
  pulse_offset = tft.textWidth(pulse_title);
  tft.setTextColor(TFT_PINK);
  tft.drawString(pulse_title, 0, 0);
  tft.setTextColor(TFT_DARKGREY);
  pulse_offset += 5;
  tft.drawString(":", pulse_offset, 0);
  pulse_offset += 10;
  // tft.setTextFont(1);
  // tft.setTextSize(2);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setFreeFont(&FreeMono12pt7b);
  tft.drawNumber(0, pulse_offset, 1);
  tft.setRotation(2);

  tft.readRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT+1, data);
  lastBPM = 0;
  lastBeatAvg = 0;
}

void pulseLoop() {
  long irValue = particleSensor.getIR();

  if (irValue > 50000) {
    if (checkForBeat(irValue) == true) {
      //We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();
      Serial.printf("delta: %d\n", delta);

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
  } 
  else {
    // no finger detected
    beatsPerMinute = 0;
    beatAvg = 0;
  }

  // remove first row (left)
  memmove(data, data+GRAPH_WIDTH, sizeof(uint16_t)*GRAPH_WIDTH*GRAPH_HEIGHT);
  // add data (right)
  addHRMDataPoints(lastBPM, beatsPerMinute, TFT_RED, 1.0);
  addHRMDataPoints(lastBeatAvg, (float) beatAvg, TFT_GREEN, 1.0);

  // send data to the screen
  tft.setRotation(2);
  tft.pushRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, data);

  Serial.printf("beatAvg: %d\n", beatAvg);
  Serial.printf("BPM: %f\n", beatsPerMinute);
  tft.setRotation(3);
  // only update if there is a change
  if (beatAvg != lastBeatAvg) {
    tft.drawString("    ", pulse_offset, 1);
    tft.drawNumber(beatAvg, pulse_offset, 1);
  }

  lastBPM = beatsPerMinute;
  lastBeatAvg = (float) beatAvg;
}

result pulsometer() {
  if (!maxDetected) {
    char* msg = "No MAX sensor detected. Aborting.";
    Serial.println(msg);
    tft.fillScreen(TFT_BLACK);
    tft.drawString(msg, tft.width() / 2, tft.height() / 2);
    delay(3000);
    resetTFT();
    return proceed;
  }

  pulseInit();
  while(!btnClick) {
    pulseLoop();
    button_loop();
  }
  // reset button
  btnClick = false;
  Serial.println("leaving pulsometer...");
  // reset
  resetTFT();
  return proceed;
}

result gameOfLife() {
  lifeInit();
  while(!btnClick) {
    lifeLoop();
    button_loop();
  }
  // reset button
  btnClick = false;
  Serial.println("leaving game of life...");
  // reset
  resetTFT();
  return proceed;
}

result scanBLE() {
  bleInit();
  while(!btnClick) {
    bleLoop();
    button_loop();
  }
  // reset button
  btnClick = false;
  Serial.println("leaving ble scanner...");
  // reset
  resetTFT();
  return proceed;
}

result scanWifi() {
  scanInit();
  while(!btnClick) {
    scanLoop();
    button_loop();
  }
  // reset button
  btnClick = false;
  Serial.println("leaving wifi scanner...");
  // reset
  resetTFT();
  return proceed;
}
