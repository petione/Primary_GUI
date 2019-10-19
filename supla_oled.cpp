#include "Arduino.h"
#define SUPLADEVICE_CPP
#include <SuplaDevice.h>

#include "supla_settings.h"
#include "supla_eeprom.h"
#include "supla_web_server.h"
#include "supla_board_settings.h"
#include "supla_oled.h"

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
#include "SH1106Wire.h"   // legacy: #include "SH1106.h"

// Initialize the OLED display using Arduino Wire:
//SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  ->supla_settings.h
SH1106Wire display(0x3c, SDA, SCL);     // ADDRESS, SDA, SCL ->supla_settings.h

// Include the UI lib
#include "OLEDDisplayUi.h"
OLEDDisplayUi ui     ( &display );

#define FRAME_DURATION 5000

FrameCallback  *frames;

int frameCount = 0;
long timeLastSwitchDHT = 0;
int dht_val = -1;

byte last_oled_state;
unsigned long time_last_oled_change;
byte oled_state = 0;
int max_frames;

void supla_oled_logo(OLEDDisplay *display) {
  display->clear();
  display->drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(supla_logo_width + 10, display->getHeight() / 2, "SUPLA");
  display->display();
}

void display_signal(OLEDDisplay *display) {
  int x = display->getWidth() - 16;
  int y = 0;
  int value = read_rssi_oled();
  //clear area only
  display->setColor(BLACK);
  display->fillRect(x, y, x + 46, 16);
  display->setColor(WHITE);
  if (value == -1) {

    display->setFont(ArialMT_Plain_10);
    display->drawString(x + 1, y, "x");

  } else {
    if (value > 0)
      display->fillRect(x, y + 6, 3, 4);
    else
      display->drawRect(x, y + 6, 3, 4);

    if (value >= 25)
      display->fillRect(x + 4, y + 4, 3, 6);
    else
      display->drawRect(x + 4, y + 4, 3, 6);

    if (value >= 50)
      display->fillRect(x + 8, y + 2, 3, 8);
    else
      display->drawRect(x + 8, y + 2, 3, 8);

    if (value >= 75)
      display->fillRect(x + 12, y, 3, 10);
    else
      display->drawRect(x + 12, y, 3, 10);

    //String s = String(value);
    // s+="%";
    //set current font size
    //display->setFont(ArialMT_Plain_10);
    //display->drawString(x+16, y, s.c_str());
  }
}

void display_relay_state(OLEDDisplay *display) {
  int y = 0;
  int x = 0;

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i = 1; i <= nr_relay; ++i) {
    byte v = digitalRead(relay_button_channel[i - 1].relay);
    if (relay_button_channel[i - 1].invert == 1) v ^= 1;
    if (v == 1) {
      display->setColor(WHITE);
      display->fillRect(x, y + 1, 10, 10);
      display->setColor(BLACK);
      display->drawString(x + 2, y, String(i));
    } else {
      display->setColor(WHITE);
      display->drawString(x + 2, y, String(i));
    }
    x += 15;
  }
  display->setColor(WHITE);
  display->drawHorizontalLine(0, 14, display->getWidth());
}

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display_signal(display);
  if (nr_relay > 0) {
    display_relay_state(display);
  }
}

void  display_supla_status(OLEDDisplay *display) {
  int x = 0;
  int y = display->getHeight() / 3;
  display->clear();

  display_signal(display);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawStringMaxWidth(x, y, display->getWidth(), String(supla_status.status_msg_oled));
  display->display();
}

void  display_config_mode(OLEDDisplay *display) {
  display->clear();
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setColor(WHITE);
  display->drawString(0, 15, "Tryb konfiguracyjny: " + String(Modul_tryb_konfiguracji));
  display->drawString(0, 28, "AP name: " + String(Config_Wifi_name));
  display->drawString(0, 41, "AP pass: "  + String(Config_Wifi_pass));
  display->drawString(0, 54, "IP: 192.168.4.1");
  display->display();
}


String get_temperature(double temperature) {
  if (temperature == -275) {
    return "error";
  } else {
    return String(temperature, 1);
  }
}

String get_humidity(double humidity) {
  if (humidity == -1) {
    return "error";
  } else {
    return String(humidity, 1);
  }
}

String get_pressure(double pressure) {
  if (pressure == -275) {
    return "error";
  } else {
    return String(floor(pressure), 0);
  }
}

void display_temperature(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 5;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(x + 0, y + drawHeightIcon, temp_width, temp_height, temp_bits);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + temp_width + 20, y + display->getHeight() / 2 - 15, ds18b20_channel[state->currentFrame].name);
  display->setFont(ArialMT_Plain_24);
  String temp = get_temperature(ds18b20_channel[state->currentFrame].last_val);
  display->drawString(x + temp_width + 10, y + drawStringIcon, temp);
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + temp_width + 10 + (temp.length() * 12), y + drawStringIcon, "ºC");
}

void display_dht_temp(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 5;
  if (millis() - timeLastSwitchDHT > FRAME_DURATION) {
    dht_val++;
    if (dht_val >= nr_dht) dht_val = 0;
    timeLastSwitchDHT = millis();
  }
  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(x + 0, y + drawHeightIcon, temp_width, temp_height, temp_bits);
  display->setFont(ArialMT_Plain_24);
  String temp = get_temperature(dht_channel[dht_val].temp);
  display->drawString(x + temp_width + 10, y + drawStringIcon , temp);
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + temp_width + 10 + (temp.length() * 12), y + drawStringIcon, "ºC");
}

void display_dht_humidity(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 5;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(x + 0, y + drawHeightIcon, humidity_width, humidity_height, humidity_bits);
  display->setFont(ArialMT_Plain_24);
  String humidity = get_humidity(dht_channel[dht_val].humidity);
  display->drawString(x + humidity_width + 10, y + drawStringIcon, humidity);
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + humidity_width + 10 + (humidity.length() * 12), y + drawStringIcon, "%");
}

void display_bme280_temp(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 5;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(x + 0, y + drawHeightIcon, temp_width, temp_height, temp_bits);
  display->setFont(ArialMT_Plain_24);
  String temp = get_temperature(bme_channel.temp);
  display->drawString(x + temp_width + 10, y + drawStringIcon , temp);
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + temp_width + 10 + (temp.length() * 12), y + drawStringIcon, "ºC");
}

void display_bme280_humidity(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 5;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(x + 0, y + drawHeightIcon, humidity_width, humidity_height, humidity_bits);
  display->setFont(ArialMT_Plain_24);
  String humidity = get_humidity(bme_channel.humidity);
  display->drawString(x + humidity_width + 10, y + drawStringIcon, humidity);
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + humidity_width + 10 + (humidity.length() * 12), y + drawStringIcon, "%");
}

void display_bme280_pressure(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  int drawHeightIcon = display->getHeight() / 2 - 10;
  int drawStringIcon = display->getHeight() / 2 - 5;

  display->setColor(WHITE);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(x + 0, y + drawHeightIcon, pressure_width, pressure_height, pressure_bits);
  display->setFont(ArialMT_Plain_24);
  String pressure = get_pressure(bme_channel.pressure_sea);
  display->drawString(x + pressure_width + 10, y + drawStringIcon, pressure);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + pressure_width + 10 + (pressure.length() * 14), y + drawStringIcon, "hPa");
}

void display_blank(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(supla_logo_width + 10, display->getHeight() / 2, "SUPLA");
}

void button_turn_oled() {
  int config_read = digitalRead(CONFIG_PIN);

  if (config_read != last_oled_state && config_read == 0 && oled_state == 1) {
    display.setBrightness(255);
    //display.displayOn();

    oled_state = 0;
    time_last_oled_change = millis();
  }

  if ((millis() - time_last_oled_change) > (max_frames * 5000) && oled_state == 0) {
    display.setBrightness(50);
    //display.displayOff();
    oled_state = 1;
  }
  last_oled_state = config_read;
}

void turn_oled() {
  if (oled_state == 1) {
    display.setBrightness(255);
    oled_state = 0;
  }
}

OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

void supla_oled_start() {

  int nr_ds18b20_pom = 0;

  for (int i = 0; i < nr_ds18b20; i++) {
    if ( ds18b20_channel[i].address != "FFFFFFFFFFFFFFFF" ) nr_ds18b20_pom++;
  }

  max_frames = nr_ds18b20_pom + nr_dht * 2 + nr_bme * 3;
  if (max_frames == 0) max_frames = 1;
  frames = (FrameCallback*)malloc(sizeof(FrameCallback) * max_frames);

  if (nr_ds18b20 > 0) {
    for (int i = 0; i < nr_ds18b20_pom; i++) {
      frames[frameCount] = {display_temperature};
      frameCount += 1;
    }
  }

  if (nr_dht > 0) {
    for (int i = 0; i < nr_dht; i++) {
      frames[frameCount] = {display_dht_temp};
      frameCount += 1;
      frames[frameCount] = {display_dht_humidity};
      frameCount += 1;
    }
  }

  if (nr_bme > 0) {
    for (int i = 0; i < nr_bme; i++) {
      frames[frameCount] = {display_bme280_temp};
      frameCount += 1;
      frames[frameCount] = {display_bme280_humidity};
      frameCount += 1;
      frames[frameCount] = {display_bme280_pressure};
      frameCount += 1;
    }
  }

  if (frameCount == 0) {
    frames[frameCount] = {display_blank};
    frameCount += 1;
  }


  if (frameCount == 1) {
    ui.setTargetFPS(60);
    ui.disableAllIndicators();
    ui.disableAutoTransition();
  } else {
    ui.setTargetFPS(60);
    ui.setActiveSymbol(activeSymbol);
    ui.setInactiveSymbol(inactiveSymbol);
    ui.setIndicatorPosition(BOTTOM);
    ui.setIndicatorDirection(LEFT_RIGHT);
    ui.setFrameAnimation(SLIDE_LEFT);
    ui.setTimePerFrame(5000);
  }
  ui.setFrames(frames, frameCount);
  ui.setOverlays(overlays, overlaysCount);
  ui.init();

  display.flipScreenVertically();
  supla_oled_logo(&display);
}

void supla_oled_timer() {

  if (nr_oled > 0) {
    if (Modul_tryb_konfiguracji != 0) {
      display_config_mode(&display);
      return;
    }
    if (supla_status.status != 17) {
      turn_oled();
      display_supla_status(&display);
      time_last_oled_change = millis();
      return;
    }
    button_turn_oled();

    int remainingTimeBudget = ui.update();

    if (remainingTimeBudget > 0) {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      delay(remainingTimeBudget);
    }
  }
}
