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

#define FRAME_DURATION 3000

typedef void (*Frame)(void);
Frame *frames;

int frameCount = 0;
long timeSinceLastModeSwitch = 0;
int frameMode = 0;

int last_oled_state = HIGH;
unsigned long time_last_oled_change;
byte oled_state = 0;

void display_signal() {
  int x = display.getWidth() - 16;
  int y = 0;
  int value = read_rssi_oled();
  //clear area only
  display.setColor(BLACK);
  display.fillRect(x, y, x + 46, 16);
  display.setColor(WHITE);
  if (value == -1) {

    display.setFont(ArialMT_Plain_10);
    display.drawString(x + 1, y, "x");

  } else {
    if (value > 0)
      display.fillRect(x, y + 6, 3, 4);
    else
      display.drawRect(x, y + 6, 3, 4);

    if (value >= 25)
      display.fillRect(x + 4, y + 4, 3, 6);
    else
      display.drawRect(x + 4, y + 4, 3, 6);

    if (value >= 50)
      display.fillRect(x + 8, y + 2, 3, 8);
    else
      display.drawRect(x + 8, y + 2, 3, 8);

    if (value >= 75)
      display.fillRect(x + 12, y, 3, 10);
    else
      display.drawRect(x + 12, y, 3, 10);

    //String s = String(value);
    // s+="%";
    //set current font size
    //display.setFont(ArialMT_Plain_10);
    //display.drawString(x+16, y, s.c_str());
  }
}

void supla_oled_logo() {
  display.clear();
  display.drawXbm(10, 17, supla_logo_width, supla_logo_height, supla_logo_bits);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(supla_logo_width + 10, display.getHeight() / 2, "SUPLA");
  display.display();
}

void  display_supla_status() {
  int x = 0;
  int y = display.getHeight() / 3;
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setColor(WHITE);
  display.drawStringMaxWidth(x, y, display.getWidth(), String(supla_status.status_msg));
  display.display();
}

void  display_config_mode() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setColor(WHITE);
  display.drawString(0, 15, "Tryb konfiguracyjny: " + String(Modul_tryb_konfiguracji));
  display.drawString(0, 28, "AP name: " + String(Config_Wifi_name));
  display.drawString(0, 41, "AP pass: "  + String(Config_Wifi_pass));
  display.drawString(0, 54, "IP: 192.168.4.1");
  display.display();
}

void display_relay_state() {
  int y = 0;
  int x = 0;

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i = 1; i <= nr_relay; ++i) {
    byte v = digitalRead(relay_button_channel[i - 1].relay);
    if (relay_button_channel[i - 1].invert == 1) v ^= 1;
    if (v == 1) {
      display.setColor(WHITE);
      display.fillRect(x, y + 1, 10, 10);
      display.setColor(BLACK);
      display.drawString(x + 2, y, String(i));
    } else {
      display.setColor(WHITE);
      display.drawString(x + 2, y, String(i));
    }
    x += 15;
  }
  display.setColor(WHITE);
  display.drawHorizontalLine(0, 14, display.getWidth());
}

String get_temperature(double temperature) {
  if (temperature == -275) {
    return "error";
  } else {
    return String(temperature, 1) + "ºC";
  }
}

String get_humidity(double humidity) {
  if (humidity == -1) {
    return "error";
  } else {
    return String(humidity, 1) + "%";
  }
}

String get_pressure(double pressure) {
  if (pressure == -275) {
    return "error";
  } else {
    return String(pressure, 1);
  }
}

void display_temperature() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawXbm(0, display.getHeight() / 2 - 10, temp_width, temp_height, temp_bits);
  display.setFont(ArialMT_Plain_10);
  display.drawString(temp_width + 20, display.getHeight() / 2 - 15, "CH" + String(frameMode));
  display.setFont(ArialMT_Plain_24);
  display.drawString(temp_width + 10, display.getHeight() / 2, get_temperature(ds18b20_channel[frameMode].last_val));
}

void display_dht_temp() {
  for (int i = 0; i < nr_dht; i++) {
    if (dht_channel[i].frameModeDHT == frameMode) {
      display.setColor(WHITE);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawXbm(0, display.getHeight() / 2 - 10, temp_width, temp_height, temp_bits);
      display.setFont(ArialMT_Plain_24);
      display.drawString(temp_width + 10, display.getHeight() / 2 , get_temperature(dht_channel[i].temp));
    }
  }
}

void display_dht_humidity() {
  for (int i = 0; i < nr_dht; i++) {
    if (dht_channel[i].frameModeDHT == frameMode - 1) {
      display.setColor(WHITE);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawXbm(0, display.getHeight() / 2 - 10, humidity_width, humidity_height, humidity_bits);
      display.setFont(ArialMT_Plain_24);
      display.drawString(humidity_width + 10, display.getHeight() / 2, get_humidity(dht_channel[i].humidity));
    }
  }
}

void display_bme280_temp() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawXbm(0, display.getHeight() / 2 - 10, temp_width, temp_height, temp_bits);
  display.setFont(ArialMT_Plain_24);
  display.drawString(temp_width + 10 , display.getHeight() / 2 , get_temperature(bme_channel.temp));
}

void display_bme280_humidity() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawXbm(0, display.getHeight() / 2 - 10, humidity_width, humidity_height, humidity_bits);
  display.setFont(ArialMT_Plain_24);
  display.drawString(humidity_width + 10, display.getHeight() / 2, get_humidity(bme_channel.humidity));
}

void display_bme280_pressure() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawXbm(0, display.getHeight() / 2 - 10, pressure_width, pressure_height, pressure_bits);
  display.setFont(ArialMT_Plain_24);
  display.drawString(pressure_width + 10, display.getHeight() / 2, get_pressure(bme_channel.pressure));
}

void button_turn_oled() {
  int config_read = digitalRead(CONFIG_PIN);
  if (config_read != last_oled_state) {
    display.setBrightness(255);
    frameMode = 0;
    timeSinceLastModeSwitch = millis();

    oled_state = 0;
    time_last_oled_change = millis();
  }
  if ((millis() - time_last_oled_change) > 20000 && oled_state == 0) {
    display.setBrightness(50);
    oled_state = 1;
  }

  last_oled_state = config_read;
}

void supla_oled_start() {
  display.init();
  display.flipScreenVertically();

  supla_oled_logo();

  int max_frames = nr_ds18b20 + nr_dht * 2 + nr_bme * 3;
  frames = (Frame*)malloc(sizeof(Frame) * max_frames);

  if (nr_ds18b20 > 1) {
    for (int i = 0; i < nr_ds18b20; i++) {
      frames[frameCount] = {display_temperature};
      frameCount += 1;
    }
  }

  if (nr_dht > 0) {
    for (int i = 0; i < nr_dht; i++) {
      dht_channel[i].frameModeDHT = frameCount;
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

}

void supla_oled_timer() {

  display.clear();
  display_signal();
  display_relay_state();

  if (Modul_tryb_konfiguracji != 0) {
    display_config_mode();
    return;
  }
  if (supla_status.status != 17) {
    display_supla_status();
    time_last_oled_change = millis();
    timeSinceLastModeSwitch = millis();
    return;
  }
  button_turn_oled();
  
  frames[frameMode]();

  display.display();

  if (millis() - timeSinceLastModeSwitch > FRAME_DURATION) {
    frameMode = (frameMode + 1)  % frameCount;

    timeSinceLastModeSwitch = millis();
  }
}
