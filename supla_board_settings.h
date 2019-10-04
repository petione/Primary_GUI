#include <DallasTemperature.h>

#ifndef SUPLA_BOARD_SETTINGS_H
#define SUPLA_BOARD_SETTINGS_H


extern int nr_button;
extern int nr_relay;
extern int nr_ds18b20;
extern int nr_dht;
extern int nr_bme;

extern int ds18b20_channel;
extern int bme_temperature_channel;
extern int bme_pressure_channel;
extern int dht_channel[];

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
  int type; //0-single 1-multi
  DeviceAddress deviceAddress;
  long lastTemperatureRequest;
  bool TemperatureRequestInProgress;

} _ds18b20_t;
extern _ds18b20_t ds18b20[];

String GetAddressToString(DeviceAddress deviceAddress);
void supla_board_configuration(void);


#endif //SUPLA_BOARD_SETTINGS_H
