/*

  ESP8266 MQTT Garage Door Controller

  https://github.com/rstms/esp8266-mqtt-door-controller
  Copyright (c) 2019 Matt Krueger
  MIT License 

  Derived from pubsubclient Library Basic ESP8266 MQTT example:
  https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino
  Copyright (c) 2008-2015 Nicholas O'Leary

 Connects to MQTT server, pulses garage door switch on command, publishes door closed sensor state
 
 Subscribes: 
   TOPIC/button
   TOPIC/state_enable
   
 Publishes:
   TOPIC/startup (on init)
   TOPIC/button (returns to 0 after 200ms pulse when set to 1)
   TOPIC/state_enable (initializes to 1)
   TOPIC/state (transmitted every 2 seconds when state_enable==1)
   
   
 serial-port-sensor
 ------------------  
   Disable Serial Port I/O - instead use serial port to sense door switch
   (connect door switch between GND and RX pins)
   set RX as INPUT_PULLUP
   read RX value as status - 0 == RX is shorted to GND
 
*/
 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WIFI conig
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASSWORD"

// MQTT config
#define MQTT_HOST "MQTT_HOSTNAME"
#define MQTT_PORT 1883
#define MQTT_USER "MQTT_USERNAME"
#define MQTT_PASS "MQTT_PASSWPORD"
#define MQTT_TOPIC(s)("MQTT_TOPIC_PREFIX/" s)

// IO config
#define RELAY0_PIN 12
#define RELAY1_PIN 13
#define RELAY2_PIN 14
#define RELAY3_PIN 16
#define RX_PIN 3

// Button press relay close duration in milliseconds
#define BUTTON_DWELL_MS 300

// use serial port for debugging or as door sense input
#define DEBUG
//#undef DEBUG

// Globals
WiFiClient espClient;
PubSubClient client(espClient);
String topic_button_command(MQTT_TOPIC("button"));
String topic_state_enable(MQTT_TOPIC("state_enable"));
int state_enable = 1;
long lastMsg = 0;

void setup_wifi() {

  delay(10);
  #ifdef DEBUG
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG
    Serial.print(".");
    #endif
  }

  randomSeed(micros());

  #ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  #endif
}

void callback(char* topic, byte* payload, unsigned int length) {

  #ifdef DEBUG
  Serial.print("[");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  #endif
  if (!topic_button_command.compareTo(topic)) {
    // Pulse the relay if an 1 was received as first character
    if ((char)payload[0] == '1') {
      #ifdef DEBUG
      Serial.print("Pressing Button...");
      #endif
      digitalWrite(RELAY0_PIN, HIGH);   // close the relay
      // stay high for BUTTON_DWELL_MS, then go low (momentary close of relay across doorbell button)
      delay(BUTTON_DWELL_MS);
      client.publish(topic_button_command.c_str(), "0");
      publish_state("Active");
      #ifdef DEBUG
      Serial.println("Released.");
      #endif
    }
    digitalWrite(RELAY0_PIN, LOW);   // open the relay
  }
  else if (!topic_state_enable.compareTo(topic)) {
    state_enable = (char)payload[0]=='1';
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.print("Attempting MQTT connection...");
    #endif
    // Create a random client ID
    String clientId = "ESP8266-";
    //clientId += String(random(0xffff), HEX);
    clientId += String(ESP.getChipId(), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      #ifdef DEBUG
      Serial.println("connected");
      #endif
      // Once connected, resubscribe...
      client.subscribe(topic_button_command.c_str());
      client.subscribe(topic_state_enable.c_str());
      // and publish an announcement...
      client.publish(MQTT_TOPIC("startup"), clientId.c_str());
      // and publish to initialize the button command and state enable
      client.publish(topic_button_command.c_str(), "0");
      client.publish(topic_state_enable.c_str(), "1");
    } else {
      #ifdef DEBUG
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      #endif
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // make sure the relays are de-energized
  pinMode(RELAY0_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  digitalWrite(RELAY0_PIN, LOW);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);

  #ifdef DEBUG
  Serial.begin(115200);
  #else
  // setup the serial port pin as a sense switch==GND detector 
  pinMode(RX_PIN, INPUT_PULLUP);
  #endif
  
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  digitalWrite(LED_BUILTIN, LOW); // light LED - setup in progress

  setup_wifi();
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);

  digitalWrite(LED_BUILTIN, HIGH); // turn LED off- setup done
}

void publish_state(const char *state) {
    lastMsg = millis();
    client.publish(MQTT_TOPIC("state"), state);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (state_enable && (millis() - lastMsg > 2000)) {
    #ifdef DEBUG
    publish_state("Debug");
    #else
    publish_state(digitalRead(RX_PIN) ? "Open" : "Closed");
    #endif
  }
}
