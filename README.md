# PlantMonitor
## Introduction
This project is a plant monitor designed to track the temperature, air humidity, and soil moisture of the plant's environment. The collected data is visualized locally using an LCD screen and simultaneously uploaded to MQTT and Telegram for online monitoring.

## Materials Needed
### Hardware Components 
+ Arduino UNO
+ Adafruit Feather HUZZAH ESP8266
+ DHT22 
+ LCD1602
+ LED
+ A stable dual-power supply device (such as a power bank) 
+ A plant 
+ Other accessories (jumper wires, resistors, data cables, etc.)

### Software Components
+ MQTT explorer (or any other MQTT software)
+ Arduino IDE
+ A mobile phone with Telegram

## Installation Guide
### ESP8266 Module
#### Module Functionality:
+ Utilize the DHT22 sensor to obtain environmental temperature and air humidity.
+ Employ the resistive voltage divider principle to acquire soil moisture.
+ Connect to WiFi and upload data to MQTT.

#### Hardware Connection: 
Connect the hardware according to the Circuit Diagram.

![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/71c61212-bd40-4444-8324-28eb40539fbb)

![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/65687e1c-69ed-4a18-9f9a-6106a1c7a7a5)

#### Software Setup:

1. Install the necessary ESP8266 environment, referring to this link: [Adafruit Feather HUZZAH ESP8266 Using Arduino IDE](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide)

      ``Don’t remember to setup libraries in your Arduino IDE before testing!``

2.	Connect to WiFi, and upload sensor data to MQTT.
  
    ![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/2edb24d4-8e19-4efc-8a18-e73af723ba31)

3. Read voltage data, convert it to soil moisture data, and upload it to MQTT.

    ![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/e42f638f-7b85-4e49-8a96-1b7d84dbde7e)

  And this is the format of my MQTT, which includes the three parameters just uploaded.
  
   ![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/7fc21a80-0ccb-4f7a-b733-82a6ee7e656c)

### Arduino UNO and LCD Module
#### Module Functionality: 
+ Utilize the I2C protocol to transmit data from ESP8266 to Arduino UNO. 
+ Display the output data on the LCD screen.

#### Hardware Connection:
1. Connect the ESP8266 and Arduino UNO. 
  + The "SDA/4" pin of the ESP8266 is connected to the "A4" pin of the Arduino UNO.
  + The "SCL/5" pin of the ESP8266 is connected to the "A5" pin of the Arduino UNO.
    
    ![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/887229a4-933f-4e24-8720-f0b11e40a7d6)

2. Connect the Arduino and the LCD screen following the diagram. Ref. [Liquid Crystal Displays (LCD) with Arduino](https://docs.arduino.cc/learn/electronics/lcd-displays)

  ![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/6e60baf3-997c-4e95-90ff-66cfb00912d8)

  ![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/550f6e18-265b-4aee-9d7a-00d44ec2626d)

#### Software Setup:

1. Write the transmission content on the ESP8266. The sample code is as follows:
  ```
    void I2C_transfer(){
      // Convert float numbers to byte arrays
      byte bytes1[sizeof(float)];
      memcpy(bytes1, &Temperature, sizeof(float));
    
      // Send byte arrays of the three parameters
      Wire.beginTransmission(ARDUINO_ADDR);
      Wire.write(bytes1, sizeof(float));
      Wire.endTransmission();
    }
  ```

2. Receive the transmitted content on Arduino UNO. The sample code is as follows (must correspond one-to-one with the parameters in the transmission code):
  ```
    void receiveEvent(){
      // Read byte arrays and convert back to float numbers
      byte bytes1[sizeof(float)];
      Wire.readBytes(bytes1, sizeof(float));
      memcpy(&Temperature, bytes1, sizeof(float));
    }
  ```

3. Write the code for the LED module on Arduino UNO to display the desired content.

### Telegram Bot Module (On ESP8266)
#### Module Functionality 
+ Utilize the Telegram Bot to read data from ESP8266
+ Send data to users via the Telegram Bot

#### Software Setup:

1. Find BotFather on Telegram, create a bot, and obtain the BOT token. 
2. Use the IDbot on Telegram to retrieve your own ID.
3. Scheduled retrieval of new messages, assessing and returning relevant parameters for the messages. 
4. Under specific circumstances, directly send warning messages to users.
   
Ref. [Telegram: Request ESP32/ESP8266 Sensor Readings (Arduino IDE)](https://randomnerdtutorials.com/telegram-request-esp32-esp8266-nodemcu-sensor-readings/)

## Instructions for Use

After everything is prepared, connect the iron nails to the circuit and insert them into the soil. Ensure that the position of each insertion is fixed so that the soil moisture reading is not affected by changes in the position of the iron nails.

Power both Arduino UNO and ESP8266. The operation of the I2C protocol requires ensuring that the supply voltage for both devices is consistent, so it is best to use the power interfaces of the same device.

Observe whether the output meets the expectations:
+ Observe whether the LCD screen outputs as expected. 
+ Check if the MQTT server has received the uploaded data. 
+ Debug the Telegram robot and observe if data can be obtained through input commands and if alerts are received.

After confirming everything is in order, wrap all your tools and equipment with a layer of plastic film. This step is to prevent any accidental damage to your devices that may occur during watering.

## Application Demonstration
This is how the plant monitor looks under normal conditions. 

![95b48f235349a4ae9fe14bbb9914df4](https://github.com/ChenRuan/PlantMonitor/assets/145383140/64cf952b-1a1b-4d95-8803-7f78e900a327)

![67e499a8510b8ae157ffa384f7c8ec4](https://github.com/ChenRuan/PlantMonitor/assets/145383140/4a0e2f43-56ed-44ad-af20-547182243896)

This is the display on the LCD screen when testing lower plant vitality (dehydration), and it also will light up the LED in the picture above. 
<img src="https://github.com/ChenRuan/PlantMonitor/assets/145383140/24a210fd-b5da-43be-838c-52d358fffad6" alt="Image" width="400"><img src="https://github.com/ChenRuan/PlantMonitor/assets/145383140/c224e415-f58c-4d10-83ad-22cd2596668b" alt="Image" width="400">

Meanwhile, the Telegram bot will receive a message like this:

![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/2ab6640c-e3f8-4246-9154-8885762d6e53)

We can also obtain information through commands:

![image](https://github.com/ChenRuan/PlantMonitor/assets/145383140/7f4772ad-b84b-4f63-bb59-35da384c1a6a)






