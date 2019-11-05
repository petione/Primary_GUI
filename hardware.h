#define ARDUINO_OTA
#define SONOFF_BASIC_CWU
//#define SONOFF_BASIC

#if defined(SONOFF_BASIC_CWU)
#define LED_CONFIG_PIN        13

#define DEFAULT_HOSTNAME      "SONOFF_BASIC_CWU"
#define RELAY_PIN             12
#define PIN_BUTTON            0
#define VIRTUAL_PIN_LOCK      99
#define CONFIG_PIN            0

#elif defined(SONOFF_BASIC)

#define DEFAULT_HOSTNAME      "SONOFF_BASIC"
#define LED_CONFIG_PIN        13
#define RELAY_PIN             12
#define PIN_BUTTON            0
#define CONFIG_PIN            0

#else

// Allow users to define new settings without migration config
//#error "UNSUPPORTED HARDWARE!"

#endif
