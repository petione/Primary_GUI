#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <DallasTemperature.h>

#define SUPLADEVICE_CPP
#include <SuplaDevice.h>

#include "ESP8266TrueRandom.h"
#include "supla_settings.h"
#include "supla_eeprom.h"
#include "supla_board_settings.h"

char GUID[SUPLA_GUID_SIZE];
byte uuidNumber[16]; // UUIDs in binary form are 16 bytes long

void czyszczenieEepromAll(void) {
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int zmienna_int = 1; zmienna_int < EEPROM_SIZE; ++zmienna_int) {
    EEPROM.write(zmienna_int, 0);
  }
  EEPROM.end();
}

void czyszczenieEeprom(void) {
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME;
  int stop = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2);

  for (int zmienna_int = 1; zmienna_int < EEPROM_SIZE; ++zmienna_int) {
    if (zmienna_int < start || zmienna_int > stop) {
      EEPROM.write(zmienna_int, 0);
    }
  }
  EEPROM.write(EEPROM_SIZE - 1, '2');
  EEPROM.end();
}

void Pokaz_zawartosc_eeprom() {

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  Serial.println("|--------------------------------------------------------------------------------------------------------------------------------------------|");
  Serial.println("|                                             HEX                                                     |                STRING                |");
  byte  eeprom = 0;
  String  eeprom_string = "";
  String znak = "";

  byte licz_wiersz = 0;
  for (int zmienna_int2 = 0; zmienna_int2 < EEPROM_SIZE; ++zmienna_int2)   {
    //ESP.wdtFeed(); // Reset watchdoga
    eeprom = (EEPROM.read(zmienna_int2));
    znak = char(EEPROM.read(zmienna_int2));
    if (znak == "") {
      eeprom_string += " ";
    } else {
      eeprom_string += znak;
    } znak = "";
    if (licz_wiersz == 0) {
      Serial.print("|   ");
      //Ser_Monitor_SHIFT("|   ");
    }
    licz_wiersz++;
    if (licz_wiersz >= 0 && licz_wiersz < 32) {
      //  ; printf("%02X", eeprom);
      //Ser_Monitor_SHIFT(("%02X", eeprom)+" ");
      Serial.print(" ");
    }
    if (licz_wiersz == 32) {
      // printf("%02X", eeprom);
      Serial.print("   |   ");
      Serial.print(eeprom_string);
      Serial.println("   |");
      //Ser_Monitor_SHIFT(("%02X", eeprom) + "   |   " + eeprom_string + "   |" );
      eeprom_string = ""; licz_wiersz = 0;
    }
  }
  Serial.println("|--------------------------------------------------------------------------------------------------------------------------------------------|");
  Serial.println("");
  Serial.println("");
}

void save_wifi_ssid(String save) {
  int start = 1;
  int koniec = start + MAX_SSID;

  int len = save.length();
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < len; ++i) {
    EEPROM.write(start + i, save[i]);
  }
  for (int i = start + len; i < koniec; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

String read_wifi_ssid(void) {
  String read_eeprom = "";
  int start = 1 ;
  int koniec = start + MAX_SSID;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_wifi_pass(String save) {
  int start = 1 + MAX_SSID;
  int koniec = start + MAX_PASSWORD;

  int len = save.length();
  if (len >= MIN_PASSWORD) {
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < len; ++i) {
      EEPROM.write(start + i, save[i]);
    }
    for (int i = start + len; i < koniec; ++i) {
      EEPROM.write(i, 0);
    }
  }
  EEPROM.end();
}

String read_wifi_pass(void) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID;
  int koniec = start + MAX_PASSWORD;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_login(String save) {
  int start = 1 + MAX_SSID + MAX_PASSWORD;
  int koniec = start + MAX_MLOGIN;
  int len = save.length();
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < len; ++i) {
    EEPROM.write(start + i, save[i]);
  }
  for (int i = start + len; i < koniec; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

String read_login(void) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID + MAX_PASSWORD;
  int koniec = start + MAX_MLOGIN;
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }

  EEPROM.end();
  return read_eeprom;
}

void save_login_pass(String save) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN;
  int koniec = start + MAX_MPASSWORD;

  int len = save.length();
  if (len >= MIN_PASSWORD) {
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < len; ++i) {
      EEPROM.write(start + i, save[i]);
    }
    for (int i = start + len; i < koniec; ++i) {
      EEPROM.write(i, 0);
    }
  }
  EEPROM.end();
}

String read_login_pass(void) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN;
  int koniec = start + MAX_MPASSWORD;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_supla_server(String save) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD;
  int koniec = start + MAX_SUPLA_SERVER;

  int len = save.length();
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < len; ++i) {
    EEPROM.write(start + i, save[i]);
  }
  for (int i = start + len; i < koniec; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

String read_supla_server(void) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD;
  int koniec = start + MAX_SUPLA_SERVER;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_supla_id(String save) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER;
  int koniec = start + MAX_SUPLA_ID;

  int len = save.length();
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < len; ++i) {
    EEPROM.write(start + i, save[i]);
  }
  for (int i = start + len; i < koniec; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

String read_supla_id(void) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER;
  int koniec = start + MAX_SUPLA_ID;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_supla_pass(String save) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID;
  int koniec = start + MAX_SUPLA_PASS;

  int len = save.length();
  if (len >= 4) {
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < len; ++i) {
      EEPROM.write(start + i, save[i]);
    }
    for (int i = start + len; i < koniec; ++i) {
      EEPROM.write(i, 0);
    }
  }
  EEPROM.end();
}

String read_supla_pass(void) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID;
  int koniec = start + MAX_SUPLA_PASS;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_supla_hostname(String save) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS;
  int koniec = start + MAX_HOSTNAME ;

  int len = save.length();
  if (len == 0) {
    save = DEFAULT_HOSTNAME;
    len = save.length();
  }
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < len; ++i) {
    EEPROM.write(start + i, save[i]);
  }
  for (int i = start + len; i < koniec; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

String read_supla_hostname(void) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS;
  int koniec = start + MAX_HOSTNAME ;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_guid(void) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME;
  int koniec = start + SUPLA_GUID_SIZE  ;

  ESP8266TrueRandom.uuid(uuidNumber);

  String uuidString = "";
  for (int i = 0; i < 16; i++) {
    int topDigit = uuidNumber[i] >> 4;
    int bottomDigit = uuidNumber[i] & 0x0f;
    uuidString += "0123456789abcdef"[topDigit];
    uuidString += "0123456789abcdef"[bottomDigit];
  }
  int len = uuidString.length();
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < len; ++i) {
    EEPROM.put(start + i, uuidString[i]);
  }
  EEPROM.end();
}

String read_guid(void) {
  String read_eeprom = "";
  int i, ii = 0;
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME;
  int koniec = start + SUPLA_GUID_SIZE;
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  String temp_read = "0x";
  for (i = start; i < koniec + 16;  i = i + 1) {
    temp_read += char(EEPROM.read(i));
    read_eeprom += char(EEPROM.read(i));
    if ( (i % 2) == 0) {
      char *_guid = strcpy((char*)malloc(temp_read.length() + 1), temp_read.c_str());
      GUID[ii] = strtoul( _guid, NULL, 16);
      temp_read = "0x";
      ii++;
    }
  }
  EEPROM.end();
  return read_eeprom;
}

void save_supla_button_type(int nr, String save) {
  if (nr <= MAX_BUTTON) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + nr;
    int len = save.length();
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < len; ++i) {
      EEPROM.write(start, save[i]);
    }
    EEPROM.end();
  }
}

int read_supla_button_type(int nr) {
  int read_eeprom;
  if (nr <= MAX_BUTTON) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + nr;
    EEPROM.begin(EEPROM_SIZE);
    delay(100);
    read_eeprom = EEPROM.read(start);
    EEPROM.end();
  }
  return read_eeprom - 48;
}

void save_supla_relay_flag(int nr, String save) {
  if (nr <= MAX_RELAY) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + nr;
    int len = save.length();
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < len; ++i) {
      EEPROM.write(start, save[i]);
    }
    EEPROM.end();
  }
}

int read_supla_relay_flag(int nr) {
  int read_eeprom;
  if (nr <= MAX_RELAY) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + nr;
    EEPROM.begin(EEPROM_SIZE);
    delay(100);
    read_eeprom = EEPROM.read(start);
    EEPROM.end();
  }
  return read_eeprom - 48;
}

void save_supla_relay_state(int nr, String save) {
  if (nr <= MAX_RELAY) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY + nr;
    int len = save.length();
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < len; ++i) {
      EEPROM.write(start, save[i]);
    }
    EEPROM.end();
  }
}

int read_supla_relay_state(int nr) {
  int read_eeprom;
  if (nr <= MAX_RELAY) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY + nr;
    EEPROM.begin(EEPROM_SIZE);
    delay(100);
    read_eeprom = EEPROM.read(start);
    EEPROM.end();
  }
  return read_eeprom - 48;
}

void save_DS18b20_address(String save, int nr) {
  if (nr <= MAX_DS18B20) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY +  MAX_RELAY_STATE + (MAX_DS18B20_SIZE * nr);
    int len = save.length();
    if (len == 0) len = 16;
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < len; ++i) {
      EEPROM.write(start + i, save[i]);
    }
    EEPROM.end();
  }
}

String read_DS18b20_address(int nr) {
  String read_eeprom = "";
  DeviceAddress deviceAddress;
  int i, ii = 0;
  if (nr <= MAX_DS18B20) {
    int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY +  MAX_RELAY_STATE + (MAX_DS18B20_SIZE * nr);
    int koniec = start + MAX_DS18B20_SIZE;
    EEPROM.begin(EEPROM_SIZE);
    delay(100);
    String temp_read = "0x";
    for (i = start; i < koniec; ++i) {
      temp_read += char(EEPROM.read(i));
      read_eeprom += char(EEPROM.read(i));
      if ( (i % 2) == 0) {
        char *_address = strcpy((char*)malloc(temp_read.length() + 1), temp_read.c_str());
        deviceAddress[ii] = strtoul( _address, NULL, 16);
        temp_read = "0x";
        ii++;
      }
    }
    memcpy(ds18b20_channel[nr].deviceAddress, deviceAddress, sizeof(deviceAddress));
    EEPROM.end();
  }
  return read_eeprom;
}

void save_DS18b20_name(String save, int nr) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY +  MAX_RELAY_STATE + (MAX_DS18B20_SIZE * MAX_DS18B20) + (MAX_DS18B20_NAME * nr);
  int koniec = start + MAX_DS18B20_NAME ;
  int len = save.length();

  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < len; ++i) {
    EEPROM.write(start + i, save[i]);
  }
  for (int i = start + len; i < koniec; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

String read_DS18b20_name(int nr) {
  String read_eeprom = "";
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY +  MAX_RELAY_STATE + (MAX_DS18B20_SIZE * MAX_DS18B20) + (MAX_DS18B20_NAME * nr);
  int koniec = start + MAX_DS18B20_NAME ;

  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  for (int i = start; i < koniec; ++i) {
    read_eeprom += char(EEPROM.read(i));
  }
  EEPROM.end();
  return read_eeprom;
}

void save_bme_elevation(int temp) {
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY +  MAX_RELAY_STATE + (MAX_DS18B20_SIZE * MAX_DS18B20) + (MAX_DS18B20_NAME * MAX_DS18B20);
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(start, temp);
  EEPROM.commit();
  EEPROM.end();
}

int read_bme_elevation() {
  int temp = 0;
  int start = 1 + MAX_SSID + MAX_PASSWORD + MAX_MLOGIN + MAX_MPASSWORD + MAX_SUPLA_SERVER + MAX_SUPLA_ID + MAX_SUPLA_PASS + MAX_HOSTNAME + (SUPLA_GUID_SIZE * 2) + MAX_BUTTON + MAX_RELAY +  MAX_RELAY_STATE + (MAX_DS18B20_SIZE * MAX_DS18B20) + (MAX_DS18B20_NAME * MAX_DS18B20);
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  EEPROM.get(start, temp);
  EEPROM.end();
  return temp;
}
