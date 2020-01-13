#include "constants.h"
#include <SPI.h>
#include <Wire.h>

#define GOL_WIDTH TFT_WIDTH
#define GOL_HEIGHT (TFT_HEIGHT-1)

size_t state_size = sizeof(uint16_t)*GOL_HEIGHT*GOL_WIDTH;
// uint16_t *data = (uint16_t*)malloc(state_size); // stores the next state of the cells
bool wins = false, red_wins=false, green_wins=false, blue_wins=false;
int count = 0, changes = 0;
const int max_count = 250;
const int min_changes = 10;
uint16_t bar_color = 1;

struct rgb {
   int8_t r, g, b;
};

rgb getRGB(uint16_t val) {
  struct rgb result;
  result.r = 0x1f & val>>11;
  result.g = 0x1f & val>>6;
  result.b = 0x1f & val;
  return result;
}

uint16_t getColor(struct rgb val) {
  if (val.r > 0x1f) val.r = 0x1f;
  if (val.g > 0x1f) val.g = 0x1f;
  if (val.b > 0x1f) val.b = 0x1f;
  return uint16_t(val.r)<<11 | uint16_t(val.g)<<6 | uint16_t(val.b);
}

void lifeInit()
{
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(&FreeMonoOblique9pt7b);
  tft.setTextColor(TFT_ORANGE);
  tft.setTextDatum(TR_DATUM);
  tft.drawString("CactusConway's", tft.width(), 0);

  tft.setFreeFont(&FreeSansBoldOblique18pt7b);
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Game", 20, 20);

  tft.setTextColor(TFT_YELLOW);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("of", (tft.width()/2)+15, tft.height()/2);

  tft.setTextColor(TFT_CYAN);
  tft.setTextDatum(BR_DATUM);
  tft.drawString("Life", tft.width()-20, tft.height()-20);
  
  delay(3000);
  tft.setRotation(0);

  data = (uint16_t*)malloc(state_size);
  wins = false, red_wins=false, green_wins=false, blue_wins=false;
  count = 0, changes = 0;
}

void randomLife() {
  int chance = 33;

  for (int r = 0; r < GOL_HEIGHT; r++) {
    for (int c = 0; c < GOL_WIDTH; c++) {
      int loc = (r*GOL_WIDTH)+c;
      int num = random(1, 100);
      if (num <= chance) {
        if (num%3 == 0) {
          data[loc] = TFT_RED;
        }
        if (num%3 == 1) {
          data[loc] = TFT_GREEN;
        }
        if (num%3 == 2) {
          data[loc] = TFT_BLUE;
        }
      } else {
        data[loc] = 0;
      }
    }
  }
}

void shiftWinValue(rgb &value) {
  if (red_wins) {
    if (value.r < 0x1f) value.r++;
    if (value.g > 0) value.g--;
    if (value.b > 0) value.b--;
  }
  if (green_wins) {
    if (value.r > 0) value.r--;
    if (value.g < 0x1f) value.g++;
    if (value.b > 0) value.b--;
  }
  if (blue_wins) {
    if (value.r > 0) value.r--;
    if (value.g > 0) value.g--;
    if (value.b < 0x1f) value.b++;
  }
}


void nextLife() {
  // next state of the cells
  uint16_t *next_state = (uint16_t*)malloc(state_size);
  changes = 0;

  for (int r = 0; r < GOL_HEIGHT; r++) {
    for (int c = 0; c < GOL_WIDTH; c++) {
      // how many live neighbors this cell has
      int liveNeighbors = 0;
      int loc = (r*GOL_WIDTH)+c;
      rgb value = getRGB(data[loc]);
      shiftWinValue(value);
      rgb avg = value;

      for (int i = -1; i <= 1; i++) {
        int y = r + i;
        if (y == -1) {
          y = GOL_HEIGHT-1;
        } else if (y == GOL_HEIGHT) {
          y = 0;
        }

        for (int j = -1; j <= 1; j++) {
          if (i == 0 && j == 0) {
            continue;
          }

          int x = c + j;
          if (x == -1) {
            x = GOL_WIDTH-1;
          } else if (x == GOL_WIDTH) {
            x = 0;
          }

          int neighbor = (y*GOL_WIDTH)+x;
          if (data[neighbor] != 0) {
            liveNeighbors++;
            rgb v = getRGB(data[neighbor]);
            avg.r += v.r;
            avg.g += v.g;
            avg.b += v.b;
          }
        }
      }

      // game of life the rules
      if (data[loc] && liveNeighbors >= 2 && liveNeighbors <= 3) {
        // live cells with 2 or 3 neighbors remain alive
        next_state[loc] = getColor(value);
      } else if (!data[loc] && liveNeighbors == 3) {
        // dead cells with 3 neighbors become alive
        avg.r = avg.r/float(liveNeighbors);
        avg.g = avg.g/float(liveNeighbors);
        avg.b = avg.b/float(liveNeighbors);
        if (!wins && count%2==0) {
          if (avg.r > avg.g && avg.r > avg.b) {
            avg.r++;
          } else if (avg.g > avg.r && avg.g > avg.b) {
            avg.g++;
          } else if (avg.b > avg.r && avg.b > avg.g) {
            avg.b++;
          }
        }
        if (wins) {
          shiftWinValue(avg);
        }
        next_state[loc] = getColor(avg);
      } else {
        // cell is dead
        if (!wins)
          next_state[loc] = 0;
        else if (data[loc] != 0) {
          next_state[loc] = data[loc];
        }
      }

      if (next_state[loc] != data[loc]) changes++;
    }
  }

  // discard the old state and keep the new one
  free(data);
  data = next_state;
}

void lifeLoop() {
  if (count >= max_count) {
    // count = 0;
    if (!wins) {
      uint32_t red=0, green=0, blue=0, total=0;
      for (int r = 0; r < GOL_HEIGHT; r++) {
        for (int c = 0; c < GOL_WIDTH; c++) {
          int loc = (r*GOL_WIDTH)+c;
          rgb value = getRGB(data[loc]);
          red += value.r;
          green += value.g;
          blue += value.b;
        }
      }
      // Serial.println(ESP.getFreeHeap());
      if (red > green && red > blue) {
        red_wins = true;
        Serial.println("Red wins.");
      } else if (green > red && green > blue) {
        green_wins = true;
        Serial.println("Green wins.");
      } else if (blue > red && blue > green) {
        blue_wins=true;
        Serial.println("Blue wins.");
      }
      wins = red_wins || green_wins || blue_wins;
    }
    // total = red + green + blue;
    // if (red_wins && (red/float(total) > .9)) {
    //   count = 0;
    // }
    // if (green_wins && (green/float(total) > .9)) {
    //   count = 0;
    // }
    // if (blue_wins && (blue/float(total) > .9)) {
    //   count = 0;
    // }

    if (red_wins && changes < min_changes) {
      count = 0;
    }
    if (green_wins && changes < min_changes) {
      count = 0;
    }
    if (blue_wins && changes < min_changes) {
      count = 0;
    }

    // Serial.print(red);
    // Serial.print("  ");
    // Serial.print(green);
    // Serial.print("  ");
    // Serial.print(blue);
    // Serial.println("  ");
  }
  if (count == 0) {
    if (wins) {
      tft.setRotation(3);
      tft.setTextColor(TFT_WHITE);
      tft.setFreeFont(&FreeSerifBold24pt7b);
      tft.setTextDatum(TL_DATUM);
      if (red_wins) {
        // tft.fillScreen(TFT_MAROON);
        tft.drawString("RED", 20, 20);
      } else if (green_wins) {
        // tft.fillScreen(TFT_DARKGREEN);
        tft.drawString("GREEN", 20, 20);
      } else if (blue_wins) {
        // tft.fillScreen(TFT_NAVY);
        tft.drawString("BLUE", 20, 20);
      }
      tft.setTextDatum(BR_DATUM);
      tft.setFreeFont(&FreeMonoBold24pt7b);
      tft.drawString("WINS!", tft.width()-20, tft.height()-20);
      tft.setRotation(0);
      delay(3000);
      wins=false, red_wins=false, green_wins=false, blue_wins=false;
    }
    randomLife();
  }
  tft.pushRect(0, 1, GOL_WIDTH, GOL_HEIGHT, data);

  nextLife();
  count++;

  if (count < max_count) {
    int progress = TFT_WIDTH*(count/float(max_count));
    int remaining = TFT_WIDTH-progress;
    int pad = progress/2;
    tft.drawLine(0, 0, pad, 0, TFT_BLACK);
    tft.drawLine(TFT_WIDTH-pad, 0, TFT_WIDTH, 0, TFT_BLACK);
    tft.drawLine(pad, 0, TFT_WIDTH-pad, 0, bar_color);
    bar_color = bar_color<<1;
    if (bar_color == 0) {
      bar_color = 1;
    }
  }
  // if (count<10) {
  //   delay(100*((10-count)/10.0));
  // }
}
