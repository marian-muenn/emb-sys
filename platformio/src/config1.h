#define CONFIG_H
// Wifi config
// const char* ssid = "Badewanne";
// const char* password = "Das.WLAN.fuer.Unterwegs";
const char *ssid = "SSID";
const char *password = "PASSWORD";
//
// MQTT Config
const char *mqtt_server = "141.57.24.186";
const char *MQTT_USER = "USER";
const char *MQTT_PW = "PASSWORD";
const int mqtt_port = 8883;
static const char *MQTT_DATA_TOPIC = "victron/1/data";
static const char *MQTT_CONFIG_TOPIC = "victron/1/config";
static const char *fingerprint PROGMEM =
    "22 DE 71 53 CC 39 FB EA 13 DC 0B EF 89 FA 9B E2 25 CD 6D 31";
static const char *client_id = "victron-esp8266-01";
