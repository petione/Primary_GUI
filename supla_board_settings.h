
#ifndef SUPLA_BOARD_SETTINGS_H
#define SUPLA_BOARD_SETTINGS_H


extern int nr_button;
extern int nr_relay;
extern int invert;
extern int nr_ds18b20;
extern int nr_dht;
extern int dht_channel[];
extern int ds18x20_channel[];
extern int relay_button_channel[];

void supla_board_configuration(void);


#endif //SUPLA_BOARD_SETTINGS_H
