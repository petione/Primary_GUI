#ifndef SUPLA_WEB_SERVER_H
#define SUPLA_WEB_SERVER_H


extern uint8_t mac[WL_MAC_ADDR_LENGTH];

extern const char* gui_color;
extern const char* gui_box_shadow;
extern const String Device_setName;

extern byte Modul_tryb_konfiguracji;

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
