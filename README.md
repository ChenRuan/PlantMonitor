# PlantMonitor
## Overview
This is the introduction of Plant Monitor. The entire device monitors plants using temperature and humidity sensors, as well as a soil moisture measurement module. It utilizes the Adafruit Feather HUZZAH ESP8266 to connect to Wi-Fi and transmit this data to MQTT for online monitoring. Additionally, it employs the I2C protocol to transmit the data to an Arduino UNO connected to an LCD for local visualization.
## Main Part (/PLANT_MONITOR)
This part primarily involves obtaining data on air humidity, temperature, and soil moisture, and transmitting it to MQTT for online monitoring. This portion of the code runs on the Adafruit Feather HUZZAH ESP8266.
## Local Visualization Output Part (/PLANT_MONITOR_OUTPUT) 
(Processing)

This part is primarily used for direct observation of the plant's growth conditions, including whether it needs watering or changing the place. Besides, I aim to create local visualization in a gamified manner, where you can not only view data like soil moisture but also see the plant's health points and experience points (based on the time it maintains a favorable growth environment).

Currently, I have learned to transfer data from ESP8266 to Arduino using the I2C protocol and now I am working on writing code to connect Arduino and an LCD to achieve a more intuitive output.
## Why I chose this plant?
![c272806d007ca9de2bf6262eda81396](https://github.com/ChenRuan/PlantMonitor/assets/145383140/cf219a51-8e7d-4d7a-bc76-65307c4ad61d)
I chose chamaedorea elegans not only because it's cheap but also because its varying water requirements at different temperatures make the entire experiment more challenging.
## Where to find my plant data
You can log in to mqtt.cetools.org and find my plant data in this directory: __student/CASA0014/plant/zczqrua/__





