#include <DallasTemperature.h>

#ifndef SUPLA_BOARD_SETTINGS_H
#define SUPLA_BOARD_SETTINGS_H

#define TYPE_SENSOR_DHT 0
#define TYPE_SENSOR_SHT 1

extern uint8_t nr_button;
extern uint8_t nr_relay;
extern uint8_t nr_ds18b20;
extern uint8_t nr_dht;
extern uint8_t nr_bme;
extern uint8_t nr_oled;

extern int ds18b20_channel_first;

extern const char* Config_Wifi_name;
extern const char* Config_Wifi_pass;

typedef struct {
  int relay;
  int channel;
  int invert;
  int DurationMS;
} _relay_button_channel;
extern _relay_button_channel relay_button_channel[];

extern DallasTemperature sensor[];
typedef struct {
  int pin;
  int channel;
  String address;
  String name;
  int type; //0-single 1-multi
  DeviceAddress deviceAddress;
  double last_val;
  unsigned long lastTemperatureRequest;
  int8_t retryCounter;
  bool iterationComplete;
} _ds18b20_channel_t;
extern _ds18b20_channel_t ds18b20_channel[];

typedef struct {
  int temperature_channel;
  int pressure_channel;
  double temp;
  double humidity;
  double pressure;
  double pressure_sea;
  int elevation;
  unsigned status;
} _bme_channel;
extern _bme_channel bme_channel;

typedef struct {
  int channel;
  double temp;
  double humidity;
  int type; //0-DHT 1-SHT
} _dht_channel;
extern _dht_channel dht_channel[];

String GetAddressToString(DeviceAddress deviceAddress);
void supla_board_configuration(void);

int supla_DigitalRead(int channelNumber, uint8_t pin);
void supla_DigitalWrite(int channelNumber, uint8_t pin, uint8_t val);

#endif //SUPLA_BOARD_SETTINGS_H
