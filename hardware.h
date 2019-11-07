//#define ARDUINO_OTA

#define SONOFF_BASIC_CWU
//#define SONOFF_BASIC
//#define SONOFF_TOUCH_2GANG



//SONOFF_BASIC_CWU *******************************************************************************
#if defined(SONOFF_BASIC_CWU)
#define LED_CONFIG_PIN        13

#define DEFAULT_HOSTNAME      "SONOFF_BASIC_CWU"
#define RELAY_PIN             12
#define BUTTON_PIN            0
#define VIRTUAL_PIN_LOCK      99
#define CONFIG_PIN            0

//SONOFF_BASIC ************************************************************************************
#elif defined(SONOFF_BASIC)

#define DEFAULT_HOSTNAME      "SONOFF_BASIC"
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

//INNE***********************************************************************************************
#else

// Allow users to define new settings without migration config
//#error "UNSUPPORTED HARDWARE!"

#endif
