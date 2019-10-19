

#ifndef SUPLA_EEPROM_H
#define SUPLA_EEPROM_H

void Pokaz_zawartosc_eeprom();
void czyszczenieEepromAll(void);
void czyszczenieEeprom(void);

void save_wifi_ssid(String save);
void save_wifi_pass(String save);
void save_login(String save);
void save_login_pass(String save);
void save_supla_server(String save);
void save_supla_id(String save);
void save_supla_pass(String save);
void save_supla_hostname(String save);
void save_guid(void);
void save_supla_button_type(int nr, String save);
void save_supla_relay_flag(int nr, String save);
void save_supla_relay_state(int nr, String save);
void save_DS18b20_address(String save, int nr);
void save_DS18b20_name(String save, int nr);
void save_bme_elevation(int temp);

String read_wifi_ssid(void);
String read_wifi_pass(void);
String read_login(void);
String read_login_pass(void);
String read_supla_server(void);
String read_supla_id(void);
String read_supla_pass(void);
String read_supla_hostname(void);
String read_guid(void);
int read_supla_button_type(int nr);
int read_supla_relay_flag(int nr);
int read_supla_relay_state(int nr);
String read_DS18b20_address(int nr);
String read_DS18b20_name(int nr);
int read_bme_elevation();

#endif //SUPLA_EEPROM_H
