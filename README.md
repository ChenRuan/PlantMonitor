# PlantMonitor
## Overview
This is the introduction of Plant Monitor. The entire device monitors plants using temperature and humidity sensors, as well as a soil moisture measurement module. It utilizes the Adafruit Feather HUZZAH ESP8266 to connect to Wi-Fi and transmit this data to MQTT for online monitoring. Additionally, it employs the I2C protocol to transmit the data to an Arduino UNO connected to an LCD for local visualization.

![8ef7f2585fd94e64cce0efe214d0d3e](https://github.com/ChenRuan/PlantMonitor/assets/145383140/78a30cac-9617-4f2f-84f8-44f256cc08b3)

## Devices
+ Adafruit Feather HUZZAH ESP8266
+ Arduino UNO
+ LCD1602
+ LED
+ Sensors
+ A plant


## Main Part (/PLANT_MONITOR)
This part primarily involves obtaining data on air humidity, temperature, and soil moisture, and transmitting it to MQTT for online monitoring. This portion of the code runs on the Adafruit Feather HUZZAH ESP8266.

It is used for quicker test:

```
void loop() {
  // handler for receiving requests to webserver
  server.handleClient();
  **// switch minuteChanged() and secondChanged() for tests
  //if(minuteChanged()){
  if (secondChanged()) {
    readMoisture();
    sendMQTT();
    I2C_transfer();
    Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
  }
  
  client.loop();
}

```
Add I2C module in ESP8266

```
#define ARDUINO_ADDR 9  // DEFINE ARDUINO I2C ADDRESS
...
uint8_t SDAPin = 4;        // on Pin 4 of the Huzzah
uint8_t SCLPin = 5;      // on Pin 5 of the Huzzah

void setup(){
...
  Wire.pins(SDAPin, SCLPin);        // Define I2C pins.
  Wire.begin();                     // Initialize I2C.
...
}

void I2C_transfer(){
  Wire.beginTransmission(ARDUINO_ADDR);
  Wire.write(Moisture);
  Serial.println("Send I2C succeed");
  Wire.endTransmission();
}
```

## Local Visualization Output Part (/PLANT_MONITOR_OUTPUT) 
(Still Processing)

This part is primarily used for direct observation of the plant's growth conditions, including whether it needs watering or changing the place. Besides, I aim to create local visualization in a gamified manner, where you can not only view data like soil moisture but also see the plant's health points and experience points (based on the time it maintains a favorable growth environment).

![41093bd8f1d95b26be7f4dd205b0d5d](https://github.com/ChenRuan/PlantMonitor/assets/145383140/adfeef56-6c1e-49ec-9b7b-219a01a4069f)

Currently, I have learned to transfer data from ESP8266 to Arduino using the I2C protocol. When the data received by Arduino indicates that the plant is in an unhealthy (lacking water) state, the red light will turn on.
```
#include <Wire.h>

#define ARDUINO_ADDR 9  // DEFINE ARDUINO I2C ADDRESS

int LED = 13; 
int data_ESP; // define received data

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  Wire.begin(ARDUINO_ADDR);
  Serial.begin(115200);
  delay(100);
}

void loop() {
  Wire.onReceive(receiveEvent);
  if( data_ESP <= 10){
    digitalWrite(LED,HIGH);
  }else{
    digitalWrite(LED,LOW);
  }
}

//this function is used to receive data and console
void receiveEvent(){
  data_ESP = Wire.read();
  Serial.println(data_ESP);
}
```
Now I am working on writing code to connect Arduino and an LCD to achieve a more intuitive output. 
## Why I chose this plant?
![c272806d007ca9de2bf6262eda81396](https://github.com/ChenRuan/PlantMonitor/assets/145383140/cf219a51-8e7d-4d7a-bc76-65307c4ad61d)
I chose chamaedorea elegans not only because it's cheap but also because its varying water requirements at different temperatures make the entire experiment more challenging.
## Where to find my plant data
You can log in to mqtt.cetools.org and find my plant data in this directory: __student/CASA0014/plant/zczqrua/__





