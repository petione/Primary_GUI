#include <DallasTemperature.h>

#ifndef SUPLA_BOARD_SETTINGS_H
#define SUPLA_BOARD_SETTINGS_H


extern int nr_button;
extern int nr_relay;
extern int nr_ds18b20;
extern int nr_dht;
extern int nr_bme;
extern int nr_oled;

extern int ds18b20_channel_first;

extern const char* Config_Wifi_name;
extern const char* Config_Wifi_pass;

typedef struct {
  int relay;
  int invert;
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
  long lastTemperatureRequest;
  bool TemperatureRequestInProgress;
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
} _bme_channel;
extern _bme_channel bme_channel;

typedef struct {
  int channel;
  double temp;
  double humidity;
} _dht_channel;
extern _dht_channel dht_channel[];

String GetAddressToString(DeviceAddress deviceAddress);
void supla_board_configuration(void);


#endif //SUPLA_BOARD_SETTINGS_H
