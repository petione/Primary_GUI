//#define ARDUINO_OTA

#define SDA 4 //-->D2
#define SCL 5 //-->D1

//#define SONOFF_BASIC_CWU
//#define SONOFF_BASIC
//#define SONOFF_TOUCH_2GANG
//#define SONOFF_TOUCH_2GANG_ESP8285
//#define SONOFF_TOUCH_3GANG_ESP8285


//SONOFF_BASIC_CWU *******************************************************************************
#if defined(SONOFF_BASIC_CWU)
#define LED_CONFIG_PIN        13

#define DEFAULT_HOSTNAME      "SONOFF BASIC CWU"
#define RELAY_PIN             12
#define BUTTON_PIN            0
#define VIRTUAL_PIN_LOCK      99
#define CONFIG_PIN            0

//SONOFF_BASIC ************************************************************************************
#elif defined(SONOFF_BASIC)

#define DEFAULT_HOSTNAME      "SONOFF BASIC"
#define LED_CONFIG_PIN        13
#define RELAY_PIN             12
#define BUTTON_PIN            0
#define CONFIG_PIN            0

//SONOFF_TOUCH_2GANG********************************************************************************
#elif defined(SONOFF_TOUCH_2GANG)

#define DEFAULT_HOSTNAME      "SONOFF TOUCH 2 GANG"

#define RELAY1_PIN            13
#define BUTTON1_PIN           12

#define RELAY2_PIN            4
#define BUTTON2_PIN           14

#define LED_CONFIG_PIN        16
#define CONFIG_PIN            14

//SONOFF_TOUCH_2GANG_ESP8285***********************************************************************
#elif defined(SONOFF_TOUCH_2GANG_ESP8285)
#define DEFAULT_HOSTNAME      "SONOFF TOUCH 2GANG"

#define RELAY1_PIN            12
#define BUTTON1_PIN           0

#define RELAY2_PIN            5
#define BUTTON2_PIN           9

#define LED_CONFIG_PIN        13
#define CONFIG_PIN            9

#define DHT22_PIN             3
#define DS18B20_PIN           3

//SONOFF_TOUCH_3GANG_ESP8285********************************************************************************
#elif defined(SONOFF_TOUCH_3GANG_ESP8285)

#define DEFAULT_HOSTNAME      "SONOFF TOUCH 3GANG"

#define RELAY1_PIN            12
#define BUTTON1_PIN           0

#define RELAY2_PIN            5
#define BUTTON2_PIN           9

#define RELAY3_PIN            4
#define BUTTON3_PIN           10

#define LED_CONFIG_PIN        13
#define CONFIG_PIN            9

#define DS18B20_PIN           3

//INNE***********************************************************************************************
#else

#define DEFAULT_HOSTNAME      "TEST"

#define RELAY1_PIN            12
#define BUTTON1_PIN           0

#define LED_CONFIG_PIN        2
#define CONFIG_PIN            0

#define DS18B20_PIN           3

// Allow users to define new settings without migration config
//#error "UNSUPPORTED HARDWARE!"

#endif