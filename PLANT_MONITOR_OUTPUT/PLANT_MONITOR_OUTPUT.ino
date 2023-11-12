#include <Wire.h>
#include <LiquidCrystal.h>  // LCD display library

#define ARDUINO_ADDR 9  // DEFINE ARDUINO I2C ADDRESS

int LED = 13; 
int data_ESP; // define received data

float Temperature = 20.0;
float Humidity = 30.0;
int Moisture = 15;
float HealthPoint = 100;
float HPSituation = 1;   // 1 means dry, 2 means wet, 0 means great

String promptMessage;

LiquidCrystal lcd(0, 1, 8, 9, 10, 11);

// create a black char
byte fullBlack[8] = {
  0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111
};

void setup() {
  pinMode(LED, OUTPUT);

  // setup I2C module
  Wire.begin(ARDUINO_ADDR);

  // setup LCD module
  lcd.begin(16, 2);
  lcd.createChar(0, fullBlack);

  // the use of serial will cause error of LCD
  //Serial.begin(115200);
  delay(100);
}

void loop() {
  // Recieve the information
  Wire.onReceive(receiveEvent);

  // LED module
  if ( HealthPoint <= 10 ) 
    {digitalWrite(LED,HIGH);}
  else 
    {digitalWrite(LED,LOW);}

  // LCD module
  LCDHPdisplay();
  if ( HealthPoint <= 10 ) 
    {LCDMessageDisplay();}
}

//this function is used to receive data
void receiveEvent(){
  // Read byte arrays and convert back to float numbers
  byte bytes1[sizeof(float)];
  byte bytes2[sizeof(float)];
  byte bytes3[sizeof(float)];
  byte bytes4[sizeof(float)];

  Wire.readBytes(bytes1, sizeof(float));
  Wire.readBytes(bytes2, sizeof(float));
  Wire.readBytes(bytes3, sizeof(float));
  Wire.readBytes(bytes4, sizeof(float));
  //Wire.readBytes((byte*)&HealthPoint, sizeof(int));
  //Wire.readBytes((byte*)&Moisture, sizeof(int));
  //Wire.readBytes((byte*)&HPSituation, sizeof(int));

  memcpy(&Temperature, bytes1, sizeof(float));
  memcpy(&Humidity, bytes2, sizeof(float));
  memcpy(&HealthPoint, bytes3, sizeof(float));
  memcpy(&HPSituation, bytes4, sizeof(float));

  if(HPSituation == 1){
    promptMessage = "Soil is too dry!";
  }else if(HPSituation == 2){
    promptMessage = "Soil is too wet!";
  }else{
    promptMessage = "  I AM GREAT!";
  }
}

void LCDHPdisplay(){
  lcd.clear();
  // Health Point part
  lcd.setCursor(0, 0);  // go to line 1, column 1
  lcd.print("HP:");

  for(int i=1 ; i <= 1 + HealthPoint/9 ; i++){
    lcd.setCursor(3+i, 0);   // go to line 1, column 4+i
    lcd.write((byte)0);   // use created black char
  }

  // Temperature and Humidity part
  lcd.setCursor(0,1);   // go to line 2, column 1
  lcd.print("TMP:");
  lcd.setCursor(4,1);   // go to line 2, column 5
  lcd.print(Temperature, 1);
  lcd.setCursor(9,1);   // go to line 2, column 9 
  lcd.print("HUM:");
  lcd.setCursor(13,1);   // go to line 2, column 13
  lcd.print(Humidity, 0);
  lcd.setCursor(15,1);   // go to line 2, column 16  
  lcd.write('%');
  delay(5000);
}

void LCDMessageDisplay(){
  lcd.clear();
  // Health Point part
  lcd.setCursor(0, 0);  // go to line 1, column 1
  lcd.print("HP:");

  for(int i=1 ; i <= 1 + HealthPoint/9 ; i++){
    lcd.setCursor(3+i, 0);   // go to line 1, column 4+i
    lcd.write((byte)0);   // use created black char
  }

  // Message part
  lcd.setCursor(0,1);   // go to line 2, column 1 
  lcd.print(promptMessage);
  delay(5000);
}


