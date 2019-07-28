# ESP8266 MQTT door controller with position sense input on serial port pin

```
  https://github.com/rstms/esp8266-mqtt-door-controller
  Copyright (c) 2019 Matt Krueger
  MIT License 
```
 
#### Derived from:
```
  pubsubclient Library Basic ESP8266 MQTT example:
  https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino
  Copyright (c) 2008-2015 Nicholas O'Leary
```

### Function
 - Connects to MQTT server
 - Pulses garage door switch on command
 - Publishes door closed sensor state

### MQTT Subscribes: 
```
TOPIC/button
TOPIC/state_enable
```
   
### MQTT Publishes:
```
TOPIC/startup (on init)
TOPIC/button (returns to 0 after 200ms pulse when set to 1)
TOPIC/state_enable (initializes to 1)
TOPIC/state (transmitted every 2 seconds when state_enable==1)
```
   
### Serial Port Sensor Input
   - Disable Serial Port I/O - instead use serial port to sense door switch
   - Connect door switch between GND and RX pins
   - Set RX as INPUT_PULLUP
   - Read RX value as status - 0 == RX is shorted to GND

### Hardware
Tested with LinkSprite's LinkNode R4 but should work with any ESP8266 relay board with modifications to the GPIO pins used. 
 - [LinkNode R4](https://www.amazon.com/LinkSprite-211201004-Arduino-Compatible-Wi-Fi-Controller/dp/B01NB0XJ0F/ref=sr_1_1?keywords=linknode+r4&qid=1564273376&s=electronics&sr=1-1)
 - [Door Position Switch](https://www.amazon.com/gp/product/B00LYD8CRK/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1) 
 - [USB to TTL Serial Programming/Debug Cable](https://www.adafruit.com/product/954)
 
### References
 - [Mosquitto MQTT Server](https://mosquitto.org/) 
 - [Arduino Client for MQTT](https://pubsubclient.knolleary.net/)
 - [ESP8266 Pinout Reference](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)
 - [MQTT Dash Android App](https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en_US)
 - [LinkSprite LinkNode R4 Documentation](http://learn.linksprite.com/linknode/linknode-r4-arduino-compatible-wifi-relay-controller/)

