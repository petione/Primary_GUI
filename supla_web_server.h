#ifndef SUPLA_WEB_SERVER_H
#define SUPLA_WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>

#define SUPLADEVICE_CPP
#include <SuplaDevice.h>

#include "supla_settings.h"
#include "supla_eeprom.h"
#include "supla_web_server.h"
#include "supla_board_settings.h"

extern uint8_t mac[WL_MAC_ADDR_LENGTH];

extern const char* gui_color;
extern const char* gui_box_shadow;
extern const String Device_setName;

extern byte Modul_tryb_konfiguracji;

typedef struct {
  int status;
  String status_msg;
  String status_msg_oled;
  String old_status_msg;
} _supla_status;
extern _supla_status supla_status;

extern bool DHCP;
void createWebServer(void *server);
String my_mac_adress(void);

String supla_webpage_start(int save);
String supla_webpage_save(void);
String supla_webpage_upddate(void);
String supla_webpage_rebot(void);
String supla_webpage_search(int save);
void status_func(int status, const char *msg);
String getLogoSupla(void);

#endif //SUPLA_WEB_SERVER_H
