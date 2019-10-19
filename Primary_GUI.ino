/* *************************************************************************

   Dzieki kolegom @wojtas567 i @Duch__ powstała ta wersja.


   Wszystkie potrzebne modyfikacja znajdują się w pliku "supla_settings.h"

 * *************************************************************************
*/


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <EEPROM.h>
#include <DoubleResetDetector.h> //Bilioteka by Stephen Denne

#define SUPLADEVICE_CPP
#include <SuplaDevice.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "supla_settings.h"
#include "supla_eeprom.h"
#include "supla_web_server.h"
#include "supla_board_settings.h"
#include "supla_oled.h"

extern "C" {
#include "user_interface.h"
}

#define DRD_TIMEOUT 5// Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0 // RTC Memory Address for the DoubleResetDetector to use
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

int nr_button = 0;
int nr_relay = 0;
int invert = 0;
int nr_ds18b20 = 0;
int nr_dht = 0;
int nr_bme = 0;
int nr_oled = 0;

_ds18b20_channel_t ds18b20_channel[MAX_DS18B20];
_relay_button_channel relay_button_channel[MAX_RELAY];
_bme_channel bme_channel;
_dht_channel dht_channel[MAX_DHT];

double temp_html;
double humidity_html;

const char* Config_Wifi_name = CONFIG_WIFI_LOGIN;
const char* Config_Wifi_pass = CONFIG_WIFI_PASSWORD;

unsigned long check_delay_WiFi = 50000;
unsigned long wait_for_WiFi;

//CONFIG
int config_state = HIGH;
int last_config_state = HIGH;
unsigned long time_last_config_change;
long config_delay = 5000;

const char* www_username;
const char* www_password;
const char* update_path = UPDATE_PATH;

WiFiClient client;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
ETSTimer led_timer;

// Setup a DHT instance
//DHT dht(DHTPIN, DHTTYPE);
DHT dht_sensor[MAX_DHT] = {
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
};

// Setup a DS18B20 instance
OneWire ds18x20[MAX_DS18B20] = 0;
//const int oneWireCount = sizeof(ds18x20) / sizeof(OneWire);
DallasTemperature sensor[MAX_DS18B20];
int ds18b20_channel_first = 0;
int dht_channel_first = 0;

//BME280***************************************************************************************************
Adafruit_BME280 bme;

//SUPLA ****************************************************************************************************
char Supla_server[MAX_SUPLA_SERVER];
char Location_id[MAX_SUPLA_ID];
char Location_Pass[MAX_SUPLA_PASS];
//*********************************************************************************************************

void setup() {
  Serial.begin(74880);
  EEPROM.begin(EEPROM_SIZE);

  client.setTimeout(500);

  if ('2' == char(EEPROM.read(EEPROM_SIZE - 1))) {
    czyszczenieEeprom();
    first_start();
  } else if ('1' != char(EEPROM.read(EEPROM_SIZE - 1))) {
    czyszczenieEepromAll();
    first_start();
    save_guid();
  }

  supla_board_configuration();

  supla_ds18b20_channel_start();
  supla_dht_start();
  supla_bme_start();

  if (drd.detectDoubleReset()) {
    drd.stop();
    gui_color = GUI_GREEN;
    Modul_tryb_konfiguracji = 2;
    Tryb_konfiguracji();
  }
  else gui_color = GUI_BLUE;

  delay(5000);
  drd.stop();

  String www_username1 = String(read_login().c_str());
  String www_password1 = String(read_login_pass().c_str());

  www_password = strcpy((char*)malloc(www_password1.length() + 1), www_password1.c_str());
  www_username = strcpy((char*)malloc(www_username1.length() + 1), www_username1.c_str());

  //  Pokaz_zawartosc_eeprom();
  read_guid();
  int Location_id = read_supla_id().toInt();
  strcpy(Supla_server, read_supla_server().c_str());
  strcpy(Location_Pass, read_supla_pass().c_str());

  if (String(read_wifi_ssid().c_str()) == 0
      || String(read_wifi_pass().c_str()) == 0
      || String(read_login().c_str()) == 0
      || String(read_login_pass().c_str()) == 0
      || String(read_supla_server().c_str()) == 0
      || String(read_supla_id().c_str()) == 0
      || String(read_supla_pass().c_str()) == 0
     ) {

    gui_color = GUI_GREEN;
    Modul_tryb_konfiguracji = 2;
    Tryb_konfiguracji();
  }

  read_guid();
  my_mac_adress();

  String supla_hostname = read_supla_hostname().c_str();
  supla_hostname.replace(" ", "-");
  WiFi.hostname(supla_hostname);
  // delete old config
  WiFi.disconnect(true);
  delay(1000);
  WiFi.onEvent(WiFiEvent);

  SuplaDevice.setStatusFuncImpl(&status_func);
  // SuplaDevice.setDigitalReadFuncImpl(&supla_DigitalRead);
  //SuplaDevice.setDigitalWriteFuncImpl(&supla_DigitalWrite);
  //SuplaDevice.setTimerFuncImpl(&supla_timer);
  SuplaDevice.setName(read_supla_hostname().c_str());

  SuplaDevice.begin(GUID,              // Global Unique Identifier
                    mac,               // Ethernet MAC address
                    Supla_server,  // SUPLA server address
                    Location_id,                 // Location ID
                    Location_Pass);

  Serial.println();
  Serial.println("Uruchamianie serwera...");

  createWebServer();

  httpUpdater.setup(&httpServer, UPDATE_PATH, www_username, www_password);
  httpServer.begin();
}

//*********************************************************************************************************

void loop() {
  if (WiFi.status() != WL_CONNECTED && Modul_tryb_konfiguracji == 0) {
    WiFi_up();
  } else {
    httpServer.handleClient();
  }

  if (Modul_tryb_konfiguracji == 0) {
    SuplaDevice.iterate();
  }

  supla_oled_timer();
  configBTN();
}
//*********************************************************************************************************

// Supla.org ethernet layer
int supla_arduino_tcp_read(void *buf, int count) {
  _supla_int_t size = client.available();

  if ( size > 0 ) {
    if ( size > count ) size = count;
    return client.read((uint8_t *)buf, size);
  }

  return -1;
}

int supla_arduino_tcp_write(void *buf, int count) {
  return client.write((const uint8_t *)buf, count);
}

bool supla_arduino_svr_connect(const char *server, int port) {
  if (WiFi.status() == WL_CONNECTED) return client.connect(server, 2015); else return false;
}

bool supla_arduino_svr_connected(void) {
  return client.connected();
}

void supla_arduino_svr_disconnect(void) {
  client.stop();
}

void supla_arduino_eth_setup(uint8_t mac[6], IPAddress *ip) {
  WiFi_up();
}

int supla_DigitalRead(int channelNumber, uint8_t pin) {

  int result = digitalRead(pin);
  /*Serial.print("Read(");
    Serial.print(pin);
    Serial.print("): ");
    Serial.println(result);*/
  return result;
}

void supla_DigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {

  /*Serial.print("Write(");
    Serial.print(pin);
    Serial.print(",");
    Serial.print(val);
    Serial.println(")");*/
  digitalWrite(pin, val);
}

void supla_timer() {

}

SuplaDeviceCallbacks supla_arduino_get_callbacks(void) {
  SuplaDeviceCallbacks cb;

  cb.tcp_read = &supla_arduino_tcp_read;
  cb.tcp_write = &supla_arduino_tcp_write;
  cb.eth_setup = &supla_arduino_eth_setup;
  cb.svr_connected = &supla_arduino_svr_connected;
  cb.svr_connect = &supla_arduino_svr_connect;
  cb.svr_disconnect = &supla_arduino_svr_disconnect;
  cb.get_temperature = &get_temperature;
  cb.get_pressure = &get_pressure;
  cb.get_temperature_and_humidity = &get_temperature_and_humidity;
  cb.get_rgbw_value = NULL;
  cb.set_rgbw_value = NULL;
  cb.read_supla_relay_state = &read_supla_relay_state;
  cb.save_supla_relay_state = &save_supla_relay_state;

  return cb;
}
//*********************************************************************************************************

void createWebServer() {

  httpServer.on("/", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_start(0));
  });

  httpServer.on("/set0", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }

    save_wifi_ssid(httpServer.arg("wifi_ssid"));
    save_wifi_pass(httpServer.arg("wifi_pass"));
    save_login( httpServer.arg("modul_login"));
    save_login_pass(httpServer.arg("modul_pass"));
    save_supla_server(httpServer.arg("supla_server"));
    save_supla_hostname(httpServer.arg("supla_hostname"));
    save_supla_id(httpServer.arg("supla_id"));
    save_supla_pass(httpServer.arg("supla_pass"));
    if (nr_button > 0) {
      for (int i = 1; i <= nr_button; ++i) {
        String button = "button_set";
        button += i;
        save_supla_button_type(i, httpServer.arg(button));
      }
    }
    if (nr_relay > 0) {
      for (int i = 1; i <= nr_relay; ++i) {
        String relay = "relay_set";
        relay += i;
        save_supla_relay_flag(i, httpServer.arg(relay));
      }
    }
    if (nr_ds18b20 > 0) {
      for (int i = 0; i < nr_ds18b20; i++) {
        String ds_name = "ds18b20_name_id_";
        ds_name += i;
        String name = httpServer.arg(ds_name);
        save_DS18b20_name(name, i);
        ds18b20_channel[i].name = name;
        if (ds18b20_channel[i].type == 1) {
          String ds_address = "ds18b20_channel_id_";
          ds_address += i;
          String address = httpServer.arg(ds_address);
          save_DS18b20_address(address, i);
          ds18b20_channel[i].address = address;
          read_DS18b20_address(i);
        }
      }
    }
    if (nr_bme > 0) {
      bme_channel.elevation = httpServer.arg("bme_elevation").toFloat();
      save_bme_elevation(bme_channel.elevation);
    }

    httpServer.send(200, "text/html", supla_webpage_start(1));
  });

  //************************************************************

  httpServer.on("/firmware_up", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_upddate());
  });

  //****************************************************************************************************************************************
  httpServer.on("/reboot", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_start(2));
    delay(100);
    resetESP();
  });
  httpServer.on("/setup", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    SetupDS18B20Multi();
    httpServer.send(200, "text/html", supla_webpage_search(1));
  });
  httpServer.on("/search", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_search(0));
  });
  httpServer.on("/eeprom", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    czyszczenieEeprom();
    httpServer.send(200, "text/html", supla_webpage_start(3));
  });
}

//****************************************************************************************************************************************
void Tryb_konfiguracji() {
  supla_led_blinking(LED_CONFIG_PIN, 100);
  my_mac_adress();
  Serial.print("Tryb konfiguracji: ");
  Serial.println(Modul_tryb_konfiguracji);

  WiFi.softAPdisconnect(true);
  delay(100);
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  WiFi.softAP(Config_Wifi_name, Config_Wifi_pass);
  delay(100);
  Serial.println("Tryb AP");
  createWebServer();
  httpServer.begin();
  Serial.println("Start Serwera");

  if (Modul_tryb_konfiguracji == 2) {
    supla_ds18b20_channel_start();
    supla_dht_start();

    while (1) {
      httpServer.handleClient();
      supla_oled_timer();
      //    SuplaDevice.iterate();
    }
  }
}

void WiFi_up() {
  if ( WiFi.status() != WL_CONNECTED
       && millis() >= wait_for_WiFi ) {

    supla_led_blinking(LED_CONFIG_PIN, 500);
    WiFi.disconnect(true);
    String esid = String(read_wifi_ssid().c_str());
    String epass = String(read_wifi_pass().c_str());
    Serial.println("WiFi init ");
    Serial.print("SSID: ");
    Serial.println(esid);
    Serial.print("PASSWORD: ");
    Serial.println(epass);

    WiFi.mode(WIFI_STA);
    WiFi.begin(esid.c_str(), epass.c_str());

    wait_for_WiFi = millis() + check_delay_WiFi;
  }
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case WIFI_EVENT_STAMODE_GOT_IP:
      //String esid = String(read_wifi_ssid().c_str());
      Serial.print("WiFi connected SSID: ");
      Serial.println(String(read_wifi_ssid().c_str()));
      Serial.print("localIP: ");
      Serial.println(WiFi.localIP());
      Serial.print("subnetMask: ");
      Serial.println(WiFi.subnetMask());
      Serial.print("gatewayIP: ");
      Serial.println(WiFi.gatewayIP());
      Serial.print("siła sygnału (RSSI): ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      Serial.println("WiFi lost connection");
      break;
  }
}

void first_start(void) {
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  EEPROM.write(EEPROM_SIZE - 1, '1');
  EEPROM.end();
  delay(100);
  save_login(DEFAULT_LOGIN);
  save_login_pass(DEFAULT_PASSWORD);
  save_supla_hostname(DEFAULT_HOSTNAME);
  save_bme_elevation(120);
}

String read_rssi(void) {
  int32_t rssi = WiFi.RSSI();
  return String(rssi) ;
}

int32_t read_rssi_oled (void) {
  int32_t rssi = WiFi.RSSI();
  if (WiFi.status() != WL_CONNECTED) return -1;
  if (rssi <= -100) return 0;
  if (rssi >= -50) return 100;
  return (2 * (rssi + 100) );
}

void get_temperature_and_humidity(int channelNumber, double * temp, double * humidity) {
  if  (channelNumber == bme_channel.temperature_channel && nr_bme != 0) {

    *temp = bme.readTemperature();
    *humidity = bme.readHumidity();

    bme_channel.temp = *temp;
    bme_channel.humidity = *humidity;

  } else {
    int i = channelNumber - dht_channel_first;
    if (dht_channel[i].channel == channelNumber) {
      *temp = dht_sensor[i].readTemperature();
      *humidity = dht_sensor[i].readHumidity();

      if ( isnan(*temp) || isnan(*humidity) ) {
        *temp = -275;
        *humidity = -1;
      }

      dht_channel[i].temp = *temp;
      dht_channel[i].humidity = *humidity;
      //  Serial.print("get_temperature_and_humidity - "); Serial.print(channelNumber); Serial.print(" -- "); Serial.print(*temp); Serial.print(" -- "); Serial.println(*humidity);
    }
  }
}

double get_pressure(int channelNumber, double last_val) {
  double pressure = -275;
  double pressure_sea = -275;

  pressure = bme.readPressure();
  pressure_sea = pressure / pow(2.718281828, - (bme_channel.elevation / ((273.15 + bme_channel.temp) * 29.263))) / 100.0F;

  bme_channel.pressure = pressure / 100.0F;
  bme_channel.pressure_sea = pressure_sea;

  return  pressure_sea;
}

double get_temperature(int channelNumber, double last_val) {
  double t = -275;

  int i = channelNumber - ds18b20_channel_first;
  // if ( sensor[i].getDeviceCount() > 0 ) {
  if ( ds18b20_channel[i].address == "FFFFFFFFFFFFFFFF" ) return -275;
  if ( millis() - ds18b20_channel[i].lastTemperatureRequest < 0) {
    ds18b20_channel[i].lastTemperatureRequest = millis();
  }

  if (ds18b20_channel[i].TemperatureRequestInProgress == false) {
    sensor[i].requestTemperaturesByAddress(ds18b20_channel[i].deviceAddress);
    ds18b20_channel[i].TemperatureRequestInProgress = true;
  }

  if ( millis() - ds18b20_channel[i].lastTemperatureRequest > 1000) {
    if ( ds18b20_channel[i].type == 0 ) {
      sensor[i].requestTemperatures();
      t = sensor[i].getTempCByIndex(0);
    } else {
      t = sensor[i].getTempC(ds18b20_channel[i].deviceAddress);
    }
    if (t == -127) t = -275;
    ds18b20_channel[i].last_val = t;
    ds18b20_channel[i].lastTemperatureRequest = millis();
    ds18b20_channel[i].TemperatureRequestInProgress = false;
  }
  // }
  return t;
}

void supla_led_blinking_func(void *timer_arg) {
  int val = digitalRead(LED_CONFIG_PIN);
  digitalWrite(LED_CONFIG_PIN, val == HIGH ? 0 : 1);
}

void supla_led_blinking(int led, int time) {

  os_timer_disarm(&led_timer);
  os_timer_setfn(&led_timer, supla_led_blinking_func, NULL);
  os_timer_arm (&led_timer, time, true);

}

void supla_led_blinking_stop(void) {
  os_timer_disarm(&led_timer);
  digitalWrite(LED_CONFIG_PIN, 1);
}

void supla_led_set(int ledPin) {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1);
}

void supla_ds18b20_channel_start(void) {
  if (nr_ds18b20 > 0 ) {
    Serial.println("DS18B2 init");
    Serial.print("Parasite power is: ");
    if ( sensor[0].isParasitePowerMode() ) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
    for (int i = 0; i < nr_ds18b20; i++) {
      sensor[i].setOneWire(&ds18x20[i]);
      sensor[i].begin();
      if (ds18b20_channel[i].type == 1) {
        sensor[i].setResolution(ds18b20_channel[i].deviceAddress, TEMPERATURE_PRECISION);
      } else {
        if (sensor[i].getAddress(ds18b20_channel[i].deviceAddress, 0)) sensor[i].setResolution(ds18b20_channel[i].deviceAddress, TEMPERATURE_PRECISION);
      }
    }
  }
}

void supla_dht_start(void) {
  if (nr_dht > 0 ) {
    for (int i = 0; i < nr_dht; i++) {
      dht_sensor[i].begin();
    }
  }
}

void supla_bme_start(void) {
  if (nr_bme > 0) {
    // Inicjalizacja BME280
    Wire.begin(SDA, SCL);

    if (!bme.begin()) { //0x76
      Serial.println("Nie znaleleziono czujnika BME280, sprawdz poprawność podłączenia i okablowanie!");
      //while (1);
    }
  }
}

void add_Sensor(int sensor) {
  SuplaDevice.addSensorNO(sensor);
}

void add_Roller_Shutter_Buttons(int channel, int button1, int button2) {
  SuplaDevice.setRollerShutterButtons(channel, button1, button2);
}

void add_Roller_Shutter_Relays(int relay1, int relay2) {
  SuplaDevice.addRollerShutterRelays(relay1, relay2);
  //  SuplaDevice.setRollerShutterFuncImpl(&supla_rs_SavePosition, &supla_rs_LoadPosition, &supla_rs_SaveSettings, &supla_rs_LoadSettings);
}

void add_Led_Config(int led) {
  supla_led_set(led);
}

void add_Config(int pin) {
  pinMode(pin, INPUT);
}

void add_Relay(int relay) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 0;
  nr_relay++;
  //SuplaDevice.addRelay(relay);
  SuplaDevice.addRelayButton(relay, -1, 0, read_supla_relay_flag(nr_relay));
}

void add_Relay_Invert(int relay) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 1;
  nr_relay++;
  //SuplaDevice.addRelay(relay, true);
  SuplaDevice.addRelayButton(relay, -1, 0, read_supla_relay_flag(nr_relay), true);
}

void add_DHT11_Thermometer(int thermpin) {
  int channel = SuplaDevice.addDHT11();
  if (nr_dht == 0) dht_channel_first = channel;

  dht_sensor[nr_dht] = { thermpin, DHT11 };
  dht_channel[nr_dht].channel = channel;
  nr_dht++;
}

void add_DHT22_Thermometer(int thermpin) {
  int channel = SuplaDevice.addDHT22();
  if (nr_dht == 0) dht_channel_first = channel;

  dht_sensor[nr_dht] = { thermpin, DHT22 };
  dht_channel[nr_dht].channel = channel;
  nr_dht++;
}

void add_DS18B20_Thermometer(int thermpin) {
  int channel = SuplaDevice.addDS18B20Thermometer();
  if (ds18b20_channel_first == 0) ds18b20_channel_first = channel;

  ds18x20[nr_ds18b20] = thermpin;
  ds18b20_channel[nr_ds18b20].pin = thermpin;
  ds18b20_channel[nr_ds18b20].channel = channel;
  ds18b20_channel[nr_ds18b20].type = 0;
  ds18b20_channel[nr_ds18b20].name = read_DS18b20_name(nr_ds18b20);
  nr_ds18b20++;
}

void add_DS18B20Multi_Thermometer(int thermpin) {
  for (int i = 0; i < MAX_DS18B20; i++) {
    int channel = SuplaDevice.addDS18B20Thermometer();
    if (i == 0) ds18b20_channel_first = channel;

    ds18x20[nr_ds18b20] = thermpin;
    ds18b20_channel[nr_ds18b20].pin = thermpin;
    ds18b20_channel[nr_ds18b20].channel = channel;
    ds18b20_channel[nr_ds18b20].type = 1;
    ds18b20_channel[nr_ds18b20].address = read_DS18b20_address(i);
    ds18b20_channel[nr_ds18b20].name = read_DS18b20_name(i);
    nr_ds18b20++;
  }
}

void add_BME280_Sensor() {
  bme_channel.pressure_channel = SuplaDevice.addPressureSensor();
  bme_channel.temperature_channel = SuplaDevice.addDHT22();

  bme_channel.elevation = read_bme_elevation();
  nr_bme++;
}

void add_Relay_Button(int relay, int button, int type) {
  return add_Relay_Button(relay, button, type, 0);
}

void add_Relay_Button_Invert(int relay, int button, int type) {
  return add_Relay_Button_Invert(relay, button, type, 0);
}

void add_Relay_Button(int relay, int button, int type, int DurationMS) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 0;
  nr_button++;
  nr_relay++;
  if (type == CHOICE_TYPE) {
    int select_button = read_supla_button_type(nr_button);
    type = select_button;
  }

  SuplaDevice.addRelayButton(relay, button, type, read_supla_relay_flag(nr_relay), DurationMS);
}

void add_Relay_Button_Invert(int relay, int button, int type, int DurationMS) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 1;
  nr_button++;
  nr_relay++;
  if (type == CHOICE_TYPE) {
    int select_button = read_supla_button_type(nr_button);
    type = select_button;
  }

  SuplaDevice.addRelayButton(relay, button, type, read_supla_relay_flag(nr_relay), true, DurationMS);
}

void add_Oled() {
  supla_oled_start();
  nr_oled ++;
}

//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress) {
  String str = "";
  for (uint8_t i = 0; i < 8; i++) {
    if ( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

void SetupDS18B20Multi() {
  DeviceAddress devAddr[MAX_DS18B20];  //An array device temperature sensors
  int numberOfDevices; //Number of temperature devices found
  numberOfDevices = sensor[0].getDeviceCount();
  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++) {
    sensor[i].requestTemperatures();
    // Search the wire for address
    if ( sensor[i].getAddress(devAddr[i], i) ) {
      Serial.print("Found device ");
      Serial.println(i, DEC);
      Serial.println("with address: " + GetAddressToString(devAddr[i]));
      Serial.println();
      save_DS18b20_address(GetAddressToString(devAddr[i]), i);
      ds18b20_channel[i].address = read_DS18b20_address(i);
    } else {
      Serial.print("Not Found device");
      Serial.print(i, DEC);
      // save_DS18b20_address("", i);
    }
    //Get resolution of DS18b20
    Serial.print("Resolution: ");
    Serial.print(sensor[i].getResolution( devAddr[i] ));
    Serial.println();

    //Read temperature from DS18b20
    float tempC = sensor[i].getTempC( devAddr[i] );
    Serial.print("Temp C: ");
    Serial.println(tempC);
  }
}

void resetESP() {
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while (1)wdt_reset();
}

void configBTN() {
  //CONFIG ****************************************************************************************************
  int config_read = digitalRead(CONFIG_PIN);
  if (config_read != last_config_state) {
    time_last_config_change = millis();
  }
  if ((millis() - time_last_config_change) > config_delay) {
    if (config_read != config_state) {
      Serial.println("Triger sate changed");
      config_state = config_read;
      if (config_state == LOW && Modul_tryb_konfiguracji != 1) {
        gui_color = GUI_GREEN;
        Modul_tryb_konfiguracji = 1;
        Tryb_konfiguracji();
        client.stop();
      } else if (config_state == LOW && Modul_tryb_konfiguracji == 1) {
        resetESP();
      }
    }
  }
  last_config_state = config_read;
}
