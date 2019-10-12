
#ifndef SUPLA_SETTINGS_H
#define SUPLA_SETTINGS_H

#include "Arduino.h"
#define SUPLADEVICE_CPP
#include <SuplaDevice.h>

#define  SUPLA_DEVICE_NAME     "SUPLA TESTY"
#define  CONFIG_WIFI_LOGIN     "MODUL SUPLA"
#define  CONFIG_WIFI_PASSWORD  ""

#define  DEFAULT_LOGIN         "admin"
#define  DEFAULT_PASSWORD      "password"
#define  DEFAULT_HOSTNAME      "Primary v2"

#define  UPDATE_PATH           "/firmware"

//DHT ****************************************************************************************************
#define  DHTTYPE               DHT22
#define  MAX_DHT               8

//DS18B20 ************************************************************************************************
#define  MAX_DS18B20            3
#define  TEMPERATURE_PRECISION  10  // rozdzielczość czujnika DS 9 -12 bit
//LED CONFIG *********************************************************************************************
#define LED_CONFIG_PIN         2
//CONFIG PIN *********************************************************************************************
#define CONFIG_PIN             0
//WIRE I2C************************************************************************************************
#define SDA                   4
#define SCL                   5

//EEPROM *************************************************************************************************
#define EEPROM_SIZE           4096/4

#define  MAX_SSID            32
#define  MAX_PASSWORD        64
#define  MAX_MLOGIN          32
#define  MAX_MPASSWORD       64
#define  MIN_PASSWORD        8
#define  MAX_SUPLA_SERVER    SUPLA_SERVER_NAME_MAXSIZE
#define  MAX_SUPLA_ID        32
#define  MAX_SUPLA_PASS      SUPLA_LOCATION_PWD_MAXSIZE
#define  MAX_HOSTNAME        32
#define  MAX_BUTTON          16
#define  MAX_RELAY           16
#define  MAX_RELAY_STATE     16
#define  MAX_DS18B20_SIZE    16

#define  GUI_BLUE              "#005c96"
#define  GUI_GREEN             "#00D151"

#define CHOICE_TYPE  -1


extern char GUID[SUPLA_GUID_SIZE];
String read_rssi(void);
int32_t read_rssi_oled (void);
void supla_led_blinking(int led, int time);
void supla_led_blinking_stop(void);


void add_Sensor(int sensor);
void add_Roller_Shutter_Buttons(int channel, int button1, int button2);
void add_Roller_Shutter_Relays(int relay1, int relay2);
void add_Led_Config(int led);
void add_Config(int pin);
void add_Relay(int relay);
void add_Relay_Invert(int relay);
void add_DHT11_Thermometer(int thermpin);
void add_DHT22_Thermometer(int thermpin);
void add_DS18B20_Thermometer(int thermpin);
void add_BME280_Sensor();
void add_Relay_Button(int relay, int button, int type);
void add_Relay_Button_Invert(int relay, int button, int type);
void add_Relay_Button(int relay, int button, int type, int DurationMS);
void add_Relay_Button_Invert(int relay, int button, int type, int DurationMS);
void add_DS18B20Multi_Thermometer(int thermpin);
void add_Oled();

double get_temperature(int channelNumber, double last_val);
void get_temperature_and_humidity(int channelNumber, double *temp, double *humidity);
double get_pressure(int channelNumber, double last_val);

extern double temp_html;
extern double humidity_html;
#endif //SUPLA_SETTINGS_H
