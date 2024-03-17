

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config1.h"
#include "vedirect.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);

time_t now;
time_t last_transmission;
String line_buffer;
String frame;
bool frame_started;
bool first_line;
bool output_state = false;
int v_th_high = 1300;
int v_th_low = 12700;
DynamicJsonDocument json_config(512);
DynamicJsonDocument json_output(1024);

void setup_wifi() {
  delay(10);
  // Connecting to WiFi network
  WiFi.begin(ssid, password);

  // Wait and retry
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a client ID
    String clientId = String(client_id);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PW)) {
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  if (String(topic).equals(String(MQTT_CONFIG_TOPIC))) {
    if (deserializeJson(json_config, (char *)payload)) {
      v_th_high = json_config["v_th_high"];
      v_th_low = json_config["v_th_low"];
    }
  }
}

void setup() {
  delay(2000);
  line_buffer = String("");
  frame = String("");
  frame_started = false;
  first_line = true;
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
  pinMode(0, OUTPUT);
  // pinMode(0, OUTPUT);// GPIO0 as output
  //  Connect to wifi
  Serial.setRxBufferSize(1024);
  Serial.begin(19200);
  //
  setup_wifi();
  // Setup mqtt with ssl and connect
  espClient.setFingerprint(fingerprint);
  client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(callback);
  reconnect();
  client.setCallback(callback);
  client.subscribe(MQTT_CONFIG_TOPIC);
  last_transmission = millis();
}

void clear_buffer() {
  while (Serial.available()) {
    Serial.read();
  }
}
// unchecked right now
bool validate_checksum() {
  unsigned int checksum = 0;
  size_t len = frame.length();
  for (size_t i = 0; i < (len - 2); i++)
    checksum = checksum + frame.charAt(i) % 255;
  if (checksum == frame.charAt(len - 3)) {
    return true;
  }
  return true;
}
void set_output(bool output_state) {
  if (output_state) {
    digitalWrite(0, HIGH);
  } else {
    digitalWrite(0, LOW);
  }
}
void update_output() {
  const char *v_str = json_output["V"];
  if (v_str == NULL)
    return;
  int voltage = String(v_str).toInt();
  if (output_state == true) {
    if (voltage < v_th_low) {
      output_state = false;
      set_output(false);
      // Serial.printf("%d\n", voltage);
      // Serial.printf("set false \n");
    }
  } else {
    if (voltage > v_th_high) {
      output_state = true;
      set_output(true);
      // Serial.printf("%d\n", voltage);
      // Serial.printf("set true \n");
    }
  }
}

bool parse_json() {
  if (deserializeJson(json_output, frame.c_str()) == DeserializationError::Ok) {
    update_output();
    return true;
  } else
    return false;
  ;
}

void process_line() {
  if (frame_started) {
    frame += line_buffer;
  }
  if (line_buffer.startsWith("Checksum")) {
    if (frame_started) {
      if (validate_checksum()) {
        frame = frame.substring(0, frame.length() - 3);
        frame.replace("\t", "\" : \"");
        frame.replace("\n", "\",\n\t\"");
        frame = "{\n\t\"" + frame + "\"\n}";
        frame.replace("\r", "");
        if (parse_json()) {
          client.publish(MQTT_DATA_TOPIC, frame.c_str());
        }
      }
      last_transmission = millis();
      frame_started = false;
      frame = "";
    } else {
      frame_started = true;
    }
  }
  line_buffer = "";
}

void process_serial() {
  while (Serial.available()) {
    char incomingByte = Serial.read();
    line_buffer += incomingByte;
    if (incomingByte == '\n') {
      process_line();
    }
  }
}

void loop() {
  now = millis();
  // WIFI and MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if ((now - last_transmission) > 8000) {
    process_serial();
  } else {
    clear_buffer();
  }
}
