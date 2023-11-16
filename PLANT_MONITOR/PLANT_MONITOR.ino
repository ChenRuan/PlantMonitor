#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezTime.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
// Libraries for Telegram Bot
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define ARDUINO_ADDR 9  // DEFINE ARDUINO I2C ADDRESS

// Sensors - DHT22 and Nails
uint8_t DHTPin = 12;        // on Pin 2 of the Huzzah
uint8_t soilPin = 0;      // ADC or A0 pin on Huzzah
uint8_t SDAPin = 4;        // on Pin 4 of the Huzzah
uint8_t SCLPin = 5;      // on Pin 5 of the Huzzah
float Temperature;
float Humidity;
int Moisture = 1; // initial value just in case web page is loaded before readMoisture called
int sensorVCC = 13;
int blueLED = 2;

// Plant data
int PlantTemp = 10;      // devide the plant temperature range
int PlantMo1min = 5;     // define the suitable plant moisture range
int PlantMo1max = 15;     
int PlantMo2min = 10;     
int PlantMo2max = 30;
float HealthPoint = 100;
float HPSituation = 0;     // 1 means dry, 2 means wet, 0 means great

DHT dht(DHTPin, DHTTYPE);   // Initialize DHT sensor.


// Wifi and MQTT
#include "arduino_secrets.h" 
/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below

#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
#define SECRET_CHATID "botID";
#define SECRET_BOTAPITOKEN "botAPItoken";
 */

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;
const char* CHAT_ID = SECRET_CHATID;
const char* BOTtoken = SECRET_BOTAPITOKEN;

ESP8266WebServer server(80);
const char* mqtt_server = "mqtt.cetools.org";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Date and time
Timezone GB;

// Telegram Bot Part
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure BOTClient;
UniversalTelegramBot bot(BOTtoken, BOTClient);
int warningCount = 720;

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void setup() {
  // Set up LED to be controllable via broker
  // Initialize the BUILTIN_LED pin as an output
  // Turn the LED off by making the voltage HIGH
  pinMode(BUILTIN_LED, OUTPUT);     
  digitalWrite(BUILTIN_LED, HIGH);  

  // Set up the outputs to control the soil sensor
  // switch and the blue LED for status indicator
  pinMode(sensorVCC, OUTPUT); 
  digitalWrite(sensorVCC, LOW);
  pinMode(blueLED, OUTPUT); 
  digitalWrite(blueLED, HIGH);

  Wire.pins(SDAPin, SCLPin);        // Define I2C pins.
  Wire.begin();                     // Initialize I2C.

  // open serial connection for debug info
  Serial.begin(115200);
  delay(100);

  // start DHT sensor
  pinMode(DHTPin, INPUT);
  dht.begin();

  // run initialisation functions
  startWifi();
  startWebserver();
  syncDate();

  // start MQTT server
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);

  // setup telegram bot
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  BOTClient.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
}

void loop() {
  // handler for receiving requests to webserver
  server.handleClient();
  // switch minuteChanged() and secondChanged() for tests
  if(minuteChanged()){
    readMoisture();
    HPCalculation();  
    sendMQTT();
    I2C_transfer();
    SendWarningMessage();
    Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
    //delay(5000);
  }

  // telegram bot part
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  client.loop();
}

void readMoisture(){
  
  // power the sensor
  digitalWrite(sensorVCC, HIGH);
  digitalWrite(blueLED, LOW);
  delay(100);
  // read the value from the sensor: change the range from (4,993) to (0,100)
  Moisture = map(analogRead(soilPin), 4,1000, 0, 100);      
  digitalWrite(sensorVCC, LOW);  
  digitalWrite(blueLED, HIGH);
  delay(100);
  Serial.print("Wet ");
  Serial.println(Moisture);   // read the value from the nails
}

void startWifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {
  // get real date and time
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());

}

void sendMQTT() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  snprintf (msg, 50, "%.1f", Temperature);
  Serial.print("Publish message for t: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/zczqrua/temperature", msg);

  Humidity = dht.readHumidity(); // Gets the values of the humidity
  snprintf (msg, 50, "%.0f", Humidity);
  Serial.print("Publish message for h: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/zczqrua/humidity", msg);

  //Moisture = analogRead(soilPin);   // moisture read by readMoisture function
  snprintf (msg, 50, "%.1i", Moisture);
  Serial.print("Publish message for m: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/zczqrua/moisture", msg);

  snprintf (msg, 50, "%.0f", HealthPoint);
  Serial.print("Publish message for hp: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/zczqrua/HealthPoint", msg);

  snprintf (msg, 50, "%.0f", HPSituation);
  Serial.print("Publish message for Situation: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/zczqrua/Situation", msg);

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/CASA0014/plant/zczqrua/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void startWebserver() {
  // when connected and IP address obtained start HTTP server  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  //Serial.println("HTTP server started");  
}

void handle_OnConnect() {
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity
  server.send(200, "text/html", SendHTML(Temperature, Humidity, Moisture));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void I2C_transfer(){
  // Convert float numbers to byte arrays
  byte bytes1[sizeof(float)];
  byte bytes2[sizeof(float)];
  byte bytes3[sizeof(float)];
  byte bytes4[sizeof(float)];
  memcpy(bytes1, &Temperature, sizeof(float));
  memcpy(bytes2, &Humidity, sizeof(float));
  memcpy(bytes3, &HealthPoint, sizeof(float));
  memcpy(bytes4, &HPSituation, sizeof(float));

  // Send byte arrays of the three parameters
  Wire.beginTransmission(ARDUINO_ADDR);
  Wire.write(bytes1, sizeof(float));
  Wire.write(bytes2, sizeof(float));
  Wire.write(bytes3, sizeof(float));
  Wire.write(bytes4, sizeof(float));
  //Wire.write((byte*)&Moisture, sizeof(int));
  //Wire.write((byte*)&HealthPoint, sizeof(int));
  //Wire.write((byte*)&HPSituation, sizeof(int));
  Wire.endTransmission();

  //Serial.println("send succeed"+HPSituation);
}

void HPCalculation(){
  // Situation 1: Temp <= PlantTemp
  // Out of range HP-1 else HP+1
  if(Temperature <= PlantTemp){
    if ( Moisture < PlantMo1min ){
      if ( HealthPoint > 0 ) 
        {HealthPoint -= 1;
         HPSituation = 1;}
    }
    else if (Moisture > PlantMo1max ){
      if ( HealthPoint > 0 ) 
        {HealthPoint -= 1;
         HPSituation = 2;}
    }
    else  {
    if ( HealthPoint < 100 )
        {HealthPoint += 1;
        HPSituation = 0;}
    }
  } 
  // Situation 2: Temp > PlantTemp
  else{
    if ( Moisture < PlantMo2min ){
      if ( HealthPoint > 0 ) 
        {HealthPoint -= 1;
         HPSituation = 1;}
    }
    else if (Moisture > PlantMo2max ){
      if ( HealthPoint > 0 ) 
        {HealthPoint -= 1;
         HPSituation = 2;}
    }
    else  {
    if ( HealthPoint < 100 )
      {HealthPoint += 1;
      HPSituation = 0;}
    }
  }
  //Serial.println("HealthPoint: "+ HealthPoint);
}

// happens when bot recieves new message
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    //Serial.println(chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/info") {
      String BOTinfo = "Here is the information from the plant monitor.\nTemperature: " + String(Temperature) + "\nHumidity: " + String(Humidity) + "\n";
      BOTinfo += "Moisture: " + String(Moisture) + "\nHealthPoint: " + String(HealthPoint) + "\n";
      if(HealthPoint <= 10 and HPSituation == 1){
        BOTinfo += "The soil is too dry and you need to water it!";
      } else if (HealthPoint <= 10 and HPSituation == 2){
        BOTinfo += "The soil is too wet and you need to drain it!";
      } else{
        BOTinfo += "Your plant is in good condition!";
      }
      bot.sendMessage(chat_id, BOTinfo, "");
    }
  }
}

void SendWarningMessage(){
    if (HealthPoint <= 10 and warningCount >= 720){
      String BOTinfo = "Here is the automatic warning from the plant monitor!\nTemperature: " + String(Temperature) + "\nHumidity: " + String(Humidity) + "\n";
      BOTinfo += "Moisture: " + String(Moisture) + "\nHealthPoint: " + String(HealthPoint) + "\n";
      if(HealthPoint <= 10 and HPSituation == 1){
        BOTinfo += "The soil is too dry and you need to water it!";
      } else if (HealthPoint <= 10 and HPSituation == 2){
        BOTinfo += "The soil is too wet and you need to drain it!";
      } else{
        BOTinfo += "Your plant is in good condition!";
      }
      bot.sendMessage(CHAT_ID, BOTinfo, ""); 
      warningCount = 0;
    } else if(HealthPoint <= 10){
      warningCount += 1;
    }
}

String SendHTML(float Temperaturestat, float Humiditystat, int Moisturestat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 DHT22 Report</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ESP8266 Huzzah DHT22 Report</h1>\n";

  ptr += "<p>Temperature: ";
  ptr += (int)Temperaturestat;
  ptr += " C</p>";
  ptr += "<p>Humidity: ";
  ptr += (int)Humiditystat;
  ptr += "%</p>";
  ptr += "<p>Moisture: ";
  ptr += Moisturestat;
  ptr += "</p>";
  ptr += "<p>Sampled on: ";
  ptr += GB.dateTime("l,");
  ptr += "<br>";
  ptr += GB.dateTime("d-M-y H:i:s T");
  ptr += "</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}