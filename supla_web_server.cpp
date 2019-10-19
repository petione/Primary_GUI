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


const char* gui_color = GUI_GREEN;
const char* gui_box_shadow = "box-shadow:0 1px 30px rgba(0,0,0,.9)";

uint8_t mac[WL_MAC_ADDR_LENGTH];

byte Modul_tryb_konfiguracji = 0;

_supla_status supla_status;
//supla_status.status_msg = "";
//supla_status.old_status_msg = "";


const char * Supported_Button[2] = {
  "Bistabilny",
  "Monostabilny"
};

const char * Supported_RelayFlag[2] = {
  "Reset",
  "Pamiętaj stan"
};

String supla_webpage_upddate(void) {
  String content = "";

  content = "<!DOCTYPE HTML>";
  content += "<meta http-equiv='content-type' content='text/html; charset=UTF-8'>";
  content += "<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  content += "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:" + String(gui_color) + ";color:#fff;line-height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px 10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:" + String(gui_color) + ";padding-right:5px}#l path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;" + String(gui_box_shadow) + "}h1,h3{margin:10px 8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px " + String(gui_color) + ";height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:" + String(gui_color) + ";line-height:41px;pointer-events:none}input,select{width:calc(100% - 145px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:144px;-webkit-appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px 2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;" + String(gui_box_shadow) + ";cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;" + String(gui_box_shadow) + ";text-align:center;font-size:26px;z-index:100}@media all and (max-height:920px){.s{margin-top:80px}}@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto 20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 12px;color:" + String(gui_color) + ";font-size:13px;position:relative;line-height:18px}input,select{width:calc(100% - 10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px " + String(gui_color) + "}select{width:100%;float:none;margin:0}}</style>";
  content += "<div class='s'>";
  content += getLogoSupla();
  content += "<h1><center>" + String(read_supla_hostname().c_str()) + "</center></h1>";
  content += "<div class='w'>";
  content += "<h3>Aktualizacja oprogramowania</h3>";
  content += "<br>";
  content += "<center>";
  content += "<iframe src=";
  content += UPDATE_PATH;
  content += ">Twoja przeglądarka nie akceptuje ramek! width='200' height='100' frameborder='100'></iframe>";
  content += "</center>";
  content += "</div>";
  content += "<a href='/'><button>POWRÓT</button></a></div>";
  content += "<br><br>";

  return content;
}

String supla_webpage_search(int save) {
  String content = "";

  content = "<!DOCTYPE HTML>";
  content += "<meta http-equiv='content-type' content='text/html; charset=UTF-8'>";
  content += "<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  content += "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:" + String(gui_color) + ";color:#fff;line-height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px 10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:" + String(gui_color) + ";padding-right:5px}#l path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;" + String(gui_box_shadow) + "}h1,h3{margin:10px 8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px " + String(gui_color) + ";height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:" + String(gui_color) + ";line-height:41px;pointer-events:none}input,select{width:calc(100% - 145px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:144px;-webkit-appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px 2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;" + String(gui_box_shadow) + ";cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;" + String(gui_box_shadow) + ";text-align:center;font-size:26px;z-index:100}@media all and (max-height:920px){.s{margin-top:80px}}@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto 20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 12px;color:" + String(gui_color) + ";font-size:13px;position:relative;line-height:18px}input,select{width:calc(100% - 10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px " + String(gui_color) + "}select{width:100%;float:none;margin:0}}</style>";
  if (save == 1) {
    content += "<div id=\"msg\" class=\"c\">Dane zapisane</div>";
  }
  content += "<div class='s'>";
  content += getLogoSupla();
  content += "<h1><center>" + String(read_supla_hostname().c_str()) + "</center></h1>";
  content += "<br>";
  content += "<center>";
  if (nr_ds18b20 > 0) {
    content += "<div class='w'>";
    content += "<h3>Temperatura</h3>";
    for (int i = 0; i < nr_ds18b20; i++) {
      double temp = get_temperature(ds18b20_channel[i].channel, 0);

      content += "<i><input name='ds18b20_channel_id_";
      content += i;
      content += "' value='" + String(ds18b20_channel[i].address.c_str()) + "' maxlength=";
      content += MAX_DS18B20_SIZE;
      content += " readonly><label>";
      if (temp != -275)content += temp;
      else content += "--.--";
      content += " <b>&deg;C</b> ";
      content += "</label>";
      content += "<label style='left:80px'>GPIO: ";
      content += String(ds18b20_channel[i].pin);
      content += "</label></i>";
    }
    content += "</div>";
  }

  DeviceAddress tempSensor;
  int numberOfDevices = 0; //Number of temperature devices found
  content += "<div class='w'>";
  content += "<h3>Znalezione DS18b20</h3>";
  numberOfDevices = sensor[0].getDeviceCount();
  if (numberOfDevices != 0) {
    for (int i = 0; i < nr_ds18b20; i++) {
      // Search the wire for address
      if ( sensor[i].getAddress(tempSensor, i) ) {
        content += "<i><input value='" + GetAddressToString(tempSensor) + "' length=";
        content += MAX_DS18B20_SIZE;
        content += " readonly><label></i>";
      }
    }
  } else {
    content += "<i><label>brak podłączonych czujników</label></i>";
  }
  content += "</div>";

  content += "</center>";
  content += "<form method='post' action='setup'>";
  content += "<button type='submit'>Zapisz znalezione DSy</button></form>";
  content += "<br>";
  content += "<a href='/'><button>Powrót</button></a></div>";
  content += "<br><br>";

  return content;
}

String supla_webpage_start(int save) {
  String content = "";

  content = "<!DOCTYPE HTML>";
  content += "<meta http-equiv='content-type' content='text/html'; charset='UTF-8'>";
  content += "<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  content += "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:" + String(gui_color) + ";color:#fff;line-height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px 10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:" + String(gui_color) + ";padding-right:5px}#l path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;" + String(gui_box_shadow) + "}h1,h3{margin:10px 8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px " + String(gui_color) + ";height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:" + String(gui_color) + ";line-height:41px;pointer-events:none}input,select{width:calc(100% - 145px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:144px;-webkit-appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px 2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;" + String(gui_box_shadow) + ";cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;" + String(gui_box_shadow) + ";text-align:center;font-size:26px;z-index:100}@media all and (max-height:920px){.s{margin-top:80px}}@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto 20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 12px;color:" + String(gui_color) + ";font-size:13px;position:relative;line-height:18px}input,select{width:calc(100% - 10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px " + String(gui_color) + "}select{width:100%;float:none;margin:0}}</style>";
  if (save == 1) {
    content += "<div id=\"msg\" class=\"c\">Dane zapisane - należy zrobić restart urządzenia!!!</div>";
  }
  else if (save == 2) {
    content += "<div id=\"msg\" class=\"c\">Restart modułu</div>";
  } else if (save == 3) {
    content += "<div id=\"msg\" class=\"c\">Dane wymazane - należy zrobić restart urządzenia!!!</div>";
  }
  content += "<script type='text/javascript'>setTimeout(function(){var element =  document.getElementById('msg');if ( element != null ) element.style.visibility = 'hidden';},3200);</script>";
  content += "<div class='s'>";
  content += getLogoSupla();
  content += "<h1><center>" + String(read_supla_hostname().c_str()) + "</center></h1>";
  content += "<font size='2'>STATUS: " + supla_status.status_msg + "</font><br>";
  content += "<font size='2'>GUID:  " + read_guid() + "</font><br>";
  content += "<font size='2'>MAC:  " + my_mac_adress() + "</font><br>";
  content += "<font size='2'>RSSI: " + read_rssi() + "</font>";
  content += "<form method='post' action='set0'>";
  content += "<div class='w'>";
  content += "<h3>Ustawienia WIFI</h3>";
  content += "<i><input name='wifi_ssid' value='" + String(read_wifi_ssid().c_str()) + "'length=";
  content += MAX_SSID;
  content += "><label>Nazwa sieci</label></i>";
  content += "<i><input name='wifi_pass' ";
  if (Modul_tryb_konfiguracji == 1 || Modul_tryb_konfiguracji == 2) {
    content += "type='password' value=''";
  }
  else {
    content += "value='" + String(read_wifi_pass().c_str()) + "'";
  }
  if (String(read_wifi_pass().c_str()) != 0) {
    content += ">";
  }
  else {
    content += "'minlength='";
    content += MIN_PASSWORD;
    content += "' required length=";
    content += MAX_PASSWORD;
    content += ">";
  }
  content += "<label>Hasło</label></i>";
  content += "<i><input name='supla_hostname' value='";
  String def = DEFAULT_HOSTNAME;
  if (def != String(read_supla_hostname().c_str())) {
    content += String(read_supla_hostname().c_str());
  }
  content += "'length=";
  content += MAX_HOSTNAME;
  content += " placeholder='Nie jest wymagana'><label>Nazwa modułu</label></i>";
  content += "</div>";
  content += "<div class='w'>";
  content += "<h3>Ustawienia administratora</h3>";
  content += "<i><input name='modul_login' value='" + String(read_login().c_str()) + "'length=";
  content += MAX_MLOGIN;
  content += "><label>Login</label></i>";
  content += "<i><input name='modul_pass' ";
  if (Modul_tryb_konfiguracji == 1 || Modul_tryb_konfiguracji == 2) {
    content += "type='password' value=''";
  }
  else {
    content += "value='" + String(read_login_pass().c_str()) + "'";
  }
  if (String(read_login_pass().c_str()) != 0) {
    content += ">";
  }
  else {
    content += "'minlength='";
    content += MIN_PASSWORD;
    content += "' required length=";
    content += MAX_MPASSWORD;
    content += ">";
  }
  content += "<label>Hasło</label></i>";
  content += "</div>";
  content += "<div class='w'>";
  content += "<h3>Ustawienia SUPLA</h3>";
  content += "<i><input name='supla_server' value='" + String(read_supla_server().c_str()) + "'length=";
  content += MAX_SUPLA_SERVER;
  content += "><label>Adres serwera</label></i>";
  content += "<i><input name='supla_id' value='" + String(read_supla_id().c_str()) + "'length=";
  content += MAX_SUPLA_ID;
  content += "><label>ID Lokalizacji</label></i>";
  content += "<i><input name='supla_pass' ";
  if (Modul_tryb_konfiguracji == 1 || Modul_tryb_konfiguracji == 2) {
    content += "type='password' value=''";
  }
  else {
    content += "value='" + String(read_supla_pass().c_str()) + "'";
  }
  if (String(read_supla_pass().c_str()) != 0) {
    content += ">";
  }
  else {
    content += "'minlength='";
    content += MIN_PASSWORD;
    content += "' required length=";
    content += MAX_SUPLA_PASS;
    content += ">";
  }
  content += "<label>Hasło</label></i>";
  content += "</div>";

  if (nr_ds18b20 > 0) {
    content += "<div class='w'>";
    content += "<h3>Temperatura</h3>";
    for (int i = 0; i < nr_ds18b20; i++) {
      double temp = get_temperature(ds18b20_channel[i].channel, 0);
      if (ds18b20_channel[i].type == 1) {
        content += "<i><input name='ds18b20_name_id_";
        content += i;
        content += "' value='" + String(ds18b20_channel[i].name.c_str()) + "' maxlength=";
        content += MAX_DS18B20_NAME;
        content += "><label>";
        content += "Nazwa ";
        content += i;
        content += "</label></i>";
        content += "<i><input name='ds18b20_channel_id_";
        content += i;
        content += "' value='" + String(ds18b20_channel[i].address.c_str()) + "' maxlength=";
        content += MAX_DS18B20_SIZE;
        content += "><label>";
        if (temp != -275)content += temp;
        else content += "--.--";
        content += " <b>&deg;C</b> ";
        content += "</label></i>";
      } else if (ds18b20_channel[i].type == 0) {
        content += "<i><input name='ds18b20_name_id_";
        content += i;
        content += "' value='" + String(ds18b20_channel[i].name.c_str()) + "' maxlength=";
        content += MAX_DS18B20_NAME;
        content += "><label>";
        content += "Nazwa ";
        content += i;
        content += "</label></i>";
        content += "<i><label>";
        if (temp != -275)content += temp;
        else content += "--.--";
        content += " <b>&deg;C</b> ";
        content += "</label></i>";
      }
    }
    content += "</div>";
  }
  if (nr_dht > 0 ) {
    content += "<div class='w'>";
    content += "<h3>Temperatura i wilgotność</h3>";
    for (int i = 0; i < nr_dht; i++) {
      get_temperature_and_humidity(dht_channel[i].channel, &temp_html, &humidity_html);
      content += "<i><label>";
      if (temp_html != -275) content += temp_html;
      else content += "--.--";
      content += "<b>&#x2103;</b>&emsp;";
      if (humidity_html != -1) content += humidity_html;
      else content += "--.--";
      content += "<b>&#37;</b>";
      content += "</label></i>";
    }
    content += "</div>";
  }
  if (nr_bme > 0 ) {
    content += "<div class='w'>";
    content += "<h3>BME280</h3>";
    get_temperature_and_humidity(bme_channel.temperature_channel, &temp_html, &humidity_html);
    double pressure = bme_channel.pressure;
    double pressure_sea = bme_channel.pressure_sea;

    content += "<i><label>";
    if (temp_html != -275) content += temp_html;
    else content += "--.--";
    content += "<b>&#x2103;</b>&emsp;";
    if (humidity_html != -1) content += humidity_html;
    else content += "--.--";
    content += "<b>&#37;</b>&emsp;";
    //  if (pressure != -275) content += pressure;
    //  else content += "--.--";
    //  content += "<b>hPa</b>";
    content += "</label></i>";
/*
    content += "<i><label>";
    content += "Ciśn. bezwzgl. ";
    if (pressure != -275) content += pressure;
    else content += "--.--";
    content += "<b>hPa </b> ";
    content += "</label></i>";

    content += "<i><label>";
    content += "Ciśnienie ";
    if (pressure_sea != -275) content += pressure_sea;
    else content += "--.--";
    content += "<b>hPa </b> ";
    content += "</label></i>";
*/

    content += "<i><input name='pom1' value='" + String(pressure) + "' readonly><label style='left: calc(100% - 255px)'>hPa</label>";
    content += "<label>";
    content += "Ciśn. bezwzgl.";
    content += "</label></i>";

    content += "<i><input name='pom1' value='" + String(pressure_sea) + "' readonly><label style='left: calc(100% - 255px)'>hPa</label>";
    content += "<label>";
    content += "Ciśnienie";
    content += "</label></i>";
    

    content += "<i><input name='bme_elevation' type='number' step='1' min='-1000' max='1000' ";
    content += "value='" + String(bme_channel.elevation) + "'><label>";
    content += "Wysokość m.n.p.m";
    content += "</label></i>";
    content += "</div>";
  }


  if (nr_button > 0 || nr_relay > 0) {
    content += "<div class='w'>";
    content += "<h3>Ustawienia modułu</h3>";
    if (nr_button > 0) {
      for (int i = 1; i <= nr_button; ++i) {
        content += "<i><label>Przycisk ";
        content += i;
        content += "</label><select name='button_set";
        content += i;
        content += "'>";

        for (int suported_button = 0; suported_button < 2; suported_button++) {
          content += "<option value='";
          content += suported_button;
          int select_button = read_supla_button_type(i);
          if (select_button == suported_button) {
            content += "' selected>";
          }
          else content += "' >";
          content += (Supported_Button[suported_button]);
        }
        content += "</select></i>";
      }
    }
    if (nr_relay > 0) {
      for (int i = 1; i <= nr_relay; ++i) {
        byte v = digitalRead(relay_button_channel[i - 1].relay);
        if (relay_button_channel[i - 1].invert == 1) v ^= 1;
        content += "<i><label ";
        content += ">Przekaźnik ";
        content += i;
        if (v == 1) content += " <font color='red' style='background-color:red'>##</font>";
        content += "</label><select name='relay_set";
        content += i;
        content += "'>";
        for (int suported_relay = 0; suported_relay < 2; suported_relay++) {
          content += "<option value='";
          content += suported_relay;
          char select_relay = read_supla_relay_flag(i);
          if (select_relay == suported_relay) {
            content += "' selected>";
          }
          else content += "' >";
          content += (Supported_RelayFlag[suported_relay]);
        }
        content += "</select></i>";
      }
    }
    content += "</div>";
  }
  content += "<button type='submit'>Zapisz</button></form>";
  content += "<br>";
  if (nr_ds18b20 > 0) {
    content += "<a href='/search'><button>Szukaj DS</button></a>";
    content += "<br><br>";
  }
  content += "<a href='/firmware_up'><button>Aktualizacja</button></a>";
  content += "<br><br>";
  content += "<form method='post' action='eeprom'>";
  content += "<button type='submit'>Wyczyść EEPROM</button></form>";
  content += "<br>";
  content += "<form method='post' action='reboot'>";
  content += "<button type='submit'>Restart</button></form></div>";
  content += "<br><br>";
  return content;
}

String my_mac_adress(void) {
  WiFi.macAddress(mac);
  return String(mac[WL_MAC_ADDR_LENGTH - 6], HEX) + ":" + String(mac[WL_MAC_ADDR_LENGTH - 5], HEX) + ":" + String(mac[WL_MAC_ADDR_LENGTH - 4], HEX) + ":" + String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) + ":" + String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + ":" + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
}

void status_func(int status, const char *msg) {
  switch (status) {
    case 2:
      supla_status.status_msg = "Już zainicjalizowane";
      supla_status.status_msg_oled = "Juz zainicjalizowane";
      break;
    case 3:
      supla_status.status_msg = "Nie przypisane CB";
      supla_status.status_msg_oled = "Nie przypisane CB";
      break;
    case 4:
      supla_status.status_msg = "Nieprawidłowy identyfikator GUID lub rejestracja urządzeń NIEAKTYWNA";
      supla_status.status_msg_oled = "Nieprawidlowy identyfikator GUID lub rejestracja urzadzen NIEAKTYWNA";
      break;
    case 5:
      supla_status.status_msg = "Nieznany adres serwera";
      supla_status.status_msg_oled = "Nieznany adres serwera";
      break;
    case 6:
      supla_status.status_msg = "Nieznany identyfikator ID";
      supla_status.status_msg_oled = "Nieznany identyfikator ID";
      break;
    case 7:
      supla_status.status_msg = "Zainicjowany";
      supla_status.status_msg_oled = "Zainicjowany";
      break;
    case 8:
      supla_status.status_msg = "Przekroczono limit kanału";
      supla_status.status_msg_oled = "Przekroczono limit kanalu";
      break;
    case 9:
      supla_status.status_msg = "Rozłączony";
      supla_status.status_msg_oled = "Rozlaczony";
      break;
    case 10:
      supla_status.status_msg = "Rejestracja w toku";
      supla_status.status_msg_oled = "Rejestracja w toku";
      break;
    case 11:
      supla_status.status_msg = "Błąd zmiennej";
      supla_status.status_msg_oled = "Blad zmiennej";
      break;
    case 12:
      supla_status.status_msg = "Błąd wersji protokołu";
      supla_status.status_msg_oled = "Blad wersji protokolu";
      break;
    case 13:
      supla_status.status_msg = "Złe poświadczenia";
      supla_status.status_msg_oled = "Zle poswiadczenia";
      break;
    case 14:
      supla_status.status_msg = "Tymczasowo niedostępne";
      supla_status.status_msg_oled = "Tymczasowo niedostepne";
      break;
    case 15:
      supla_status.status_msg = "Konflikt lokalizacji";
      supla_status.status_msg_oled = "Konflikt lokalizacji";
      break;
    case 16:
      supla_status.status_msg = "Konflikt kanałów";
      supla_status.status_msg_oled = "Konflikt kanalow";
      break;
    case 17:
      supla_status.status_msg = "Zarejestrowany i gotowy";
      supla_status.status_msg_oled = "Zarejestrowany i gotowy";
      break;
    case 18:
      supla_status.status_msg = "Urządzenie jest rozłączone";
      supla_status.status_msg_oled = "Urzadzenie jest rozlaczone";
      break;
    case 19:
      supla_status.status_msg = "Lokalizacja jest wyłączona";
      supla_status.status_msg_oled = "Lokalizacja jest wylaczona";
      break;
    case 20:
      supla_status.status_msg = "Przekroczono limit urządzeń";
      supla_status.status_msg_oled = "Przekroczono limit urzadzen"; break;
  }

  static int lock;
  if (status == 17) {
    supla_led_blinking_stop();
    lock = 0;
  }
  else if (status != 17 && lock == 0) {
    supla_led_blinking(LED_CONFIG_PIN, 500);
    lock = 1;
  }

  if (supla_status.old_status_msg != supla_status.status_msg) {
    Serial.println(supla_status.status_msg);
    supla_status.old_status_msg = supla_status.status_msg;
    supla_status.status = status;
  }
}

String getLogoSupla(void) {
  return "<center><a href='/'><svg version='1.1' id='l' x='0' y='0' viewBox='0 0 200 200' xml:space='preserve'><path d='M59.3,2.5c18.1,0.6,31.8,8,40.2,23.5c3.1,5.7,4.3,11.9,4.1,18.3c-0.1,3.6-0.7,7.1-1.9,10.6c-0.2,0.7-0.1,1.1,0.6,1.5c12.8,7.7,25.5,15.4,38.3,23c2.9,1.7,5.8,3.4,8.7,5.3c1,0.6,1.6,0.6,2.5-0.1c4.5-3.6,9.8-5.3,15.7-5.4c12.5-0.1,22.9,7.9,25.2,19c1.9,9.2-2.9,19.2-11.8,23.9c-8.4,4.5-16.9,4.5-25.5,0.2c-0.7-0.3-1-0.2-1.5,0.3c-4.8,4.9-9.7,9.8-14.5,14.6c-5.3,5.3-10.6,10.7-15.9,16c-1.8,1.8-3.6,3.7-5.4,5.4c-0.7,0.6-0.6,1,0,1.6c3.6,3.4,5.8,7.5,6.2,12.2c0.7,7.7-2.2,14-8.8,18.5c-12.3,8.6-30.3,3.5-35-10.4c-2.8-8.4,0.6-17.7,8.6-22.8c0.9-0.6,1.1-1,0.8-2c-2-6.2-4.4-12.4-6.6-18.6c-6.3-17.6-12.7-35.1-19-52.7c-0.2-0.7-0.5-1-1.4-0.9c-12.5,0.7-23.6-2.6-33-10.4c-8-6.6-12.9-15-14.2-25c-1.5-11.5,1.7-21.9,9.6-30.7C32.5,8.9,42.2,4.2,53.7,2.7c0.7-0.1,1.5-0.2,2.2-0.2C57,2.4,58.2,2.5,59.3,2.5z M76.5,81c0,0.1,0.1,0.3,0.1,0.6c1.6,6.3,3.2,12.6,4.7,18.9c4.5,17.7,8.9,35.5,13.3,53.2c0.2,0.9,0.6,1.1,1.6,0.9c5.4-1.2,10.7-0.8,15.7,1.6c0.8,0.4,1.2,0.3,1.7-0.4c11.2-12.9,22.5-25.7,33.4-38.7c0.5-0.6,0.4-1,0-1.6c-5.6-7.9-6.1-16.1-1.3-24.5c0.5-0.8,0.3-1.1-0.5-1.6c-9.1-4.7-18.1-9.3-27.2-14c-6.8-3.5-13.5-7-20.3-10.5c-0.7-0.4-1.1-0.3-1.6,0.4c-1.3,1.8-2.7,3.5-4.3,5.1c-4.2,4.2-9.1,7.4-14.7,9.7C76.9,80.3,76.4,80.3,76.5,81z M89,42.6c0.1-2.5-0.4-5.4-1.5-8.1C83,23.1,74.2,16.9,61.7,15.8c-10-0.9-18.6,2.4-25.3,9.7c-8.4,9-9.3,22.4-2.2,32.4c6.8,9.6,19.1,14.2,31.4,11.9C79.2,67.1,89,55.9,89,42.6z M102.1,188.6c0.6,0.1,1.5-0.1,2.4-0.2c9.5-1.4,15.3-10.9,11.6-19.2c-2.6-5.9-9.4-9.6-16.8-8.6c-8.3,1.2-14.1,8.9-12.4,16.6C88.2,183.9,94.4,188.6,102.1,188.6z M167.7,88.5c-1,0-2.1,0.1-3.1,0.3c-9,1.7-14.2,10.6-10.8,18.6c2.9,6.8,11.4,10.3,19,7.8c7.1-2.3,11.1-9.1,9.6-15.9C180.9,93,174.8,88.5,167.7,88.5z'/></svg></a></center>";
}
