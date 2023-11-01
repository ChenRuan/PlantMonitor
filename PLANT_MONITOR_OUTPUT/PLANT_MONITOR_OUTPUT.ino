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