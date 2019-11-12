//  CHOICE_TYPE 0-BISTABILNY 1-MONOSTABILNY
//  add_Relay_Button(13, 0, CHOICE_TYPE);
//  add_Relay_Button_Invert(13, 12, CHOICE_TYPE);

//  Czas załączenia przekaźnika działa tylko dla przycisku MONOSTABILNEGO
//  add_Relay_Button(5, 14, CHOICE_TYPE, 5000);
//  add_Relay_Button_Invert(13, 12, CHOICE_TYPE, 5000);

//  add_Relay(5);
//  add_Relay_Invert(5);

//  add_Roller_Shutter_Relays(5, 13) ;
//  add_Roller_Shutter_Buttons(0, 14, 12);
//  SuplaDevice.setRollerShutterFuncImpl(&supla_rs_SavePosition, &supla_rs_LoadPosition, &supla_rs_SaveSettings, &supla_rs_LoadSettings);

//  add_Sensor(4);
//  add_Sensor(16);

// add_DS18B20Multi_Thermometer(12);
// add_DS18B20_Thermometer(12);
// add_DHT11_Thermometer(12);
// add_DHT22_Thermometer(4);
// add_BME280_Sensor(); //SDA GPIO4; SCL GPIO5 -->supla_settings.h

// add_Oled(); //SDA GPIO4; SCL GPIO5 -->supla_settings.h
// add_Led_Config(LED_CONFIG_PIN);
// add_Led_Config_Invert(LED_CONFIG_PIN);
// add_Config(CONFIG_PIN);

#include <Arduino.h>

#define SUPLADEVICE_CPP
#include <SuplaDevice.h>

#include "supla_settings.h"
#include "supla_eeprom.h"
#include "supla_web_server.h"
#include "supla_board_settings.h"
#include "supla_oled.h"
#include "hardware.h"

extern "C" {
#include "user_interface.h"
}

void supla_board_configuration(void) {

  //SONOFF_BASIC_CWU *****************************************************************************
#if defined(SONOFF_BASIC_CWU)

  add_Relay_Button(RELAY_PIN, BUTTON_PIN, 1);
  add_Relay(VIRTUAL_PIN_LOCK);
  add_Led_Config(LED_CONFIG_PIN);
  add_Config(CONFIG_PIN);

  SuplaDevice.setDigitalReadFuncImpl(&supla_DigitalRead);
  SuplaDevice.setDigitalWriteFuncImpl(&supla_DigitalWrite);

  //SONOFF_BASIC *********************************************************************************
#elif defined(SONOFF_BASIC)

  add_Relay_Button(RELAY_PIN, BUTTON_PIN, CHOICE_TYPE);
  add_Led_Config(LED_CONFIG_PIN);
  add_Config(CONFIG_PIN);

  //SONOFF_TOUCH_2GANG*****************************************************************************
#elif defined(SONOFF_TOUCH_2GANG)

  add_Relay_Button(RELAY1_PIN, BUTTON1_PIN, CHOICE_TYPE);
  add_Relay_Button(RELAY2_PIN, BUTTON2_PIN, CHOICE_TYPE);
  //add_Led_Config(LED_CONFIG_PIN);
  add_Config(CONFIG_PIN);

  //SONOFF_TOUCH_3GANG_ESP8285***********************************************************************
#elif defined(SONOFF_TOUCH_3GANG_ESP8285)

  add_Relay_Button(RELAY1_PIN, BUTTON1_PIN, 1);
  add_Relay_Button(RELAY2_PIN, BUTTON2_PIN, 1);
  add_Relay_Button(RELAY3_PIN, BUTTON3_PIN, 1);
  add_DS18B20_Thermometer(DS18B20_PIN);
  add_Led_Config_Invert(LED_CONFIG_PIN);
  add_Config(CONFIG_PIN);

#else
  add_Relay_Button(RELAY1_PIN, BUTTON1_PIN, CHOICE_TYPE);

  add_Led_Config(LED_CONFIG_PIN);
  add_Config(CONFIG_PIN);
  // Allow users to define new settings without migration config
  //#error "UNSUPPORTED HARDWARE!"

#endif
}

//SONOFF_BASIC_CWU *******************************************************************************
#if defined(SONOFF_BASIC_CWU)
uint8_t state_lock = 1;

int supla_DigitalRead(int channelNumber, uint8_t pin) {
  if (pin == VIRTUAL_PIN_LOCK) return state_lock;

  if (pin == RELAY_PIN && state_lock == 1) return digitalRead(pin);

  if (pin == BUTTON_PIN) return digitalRead(pin);
}

void supla_DigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {
  if (pin == VIRTUAL_PIN_LOCK) {
    state_lock = 1;
    SuplaDevice.channelValueChanged(channelNumber, val);
    val ? SuplaDevice.relayOn(0, 0) : SuplaDevice.relayOff(0);
    state_lock = val;
  }

  if (pin == RELAY_PIN && state_lock == 1) {
    digitalWrite(LED_CONFIG_PIN, !val);
    digitalWrite(RELAY_PIN, val);
  }

  if (pin == BUTTON_PIN) digitalWrite(BUTTON_PIN, val);
}

#else

// Allow users to define new settings without migration config
//#error "UNSUPPORTED HARDWARE!"

#endif
