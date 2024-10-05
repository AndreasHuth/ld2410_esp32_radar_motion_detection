# My LD2410 motion sensor
## Brief description
This project is based on the HLK-LD2410 radar sensor module.
The LD2410 is a human presence detecting radar - 24GHz.

Sensor product description (source: https://hlktech.net/index.php?id=988):
LD2410 is a high-sensitivity 24GHz human presence status sensing moduledeveloped by Hi-link. Its working principle is to use FMCW frequency-modulated continuous waves to detect human targets in the set space. Combined with radar signal processing and precise human body sensing algorithms, it realizes high-sensitivity human presence status sensing, and can identify human bodies in motion and stationary states. And auxiliary information such as the distance of the target can be calculated.

The project combines a 
- LD2410 module
- ESP32 D1 Mini NodeMCU module
- DS18B20 - digital temperature sensor
- LDR - as a light sensor
- Status LED

The motion sensor is supplied with power via the USB port of the ESP32 module.

A special PCB was developed for the electronics (please contact me if you have any questions). The circuit diagram can also be found here:
https://github.com/AndreasHuth/ld2410_esp32_radar_motion_detection/tree/master/schematic

To use the motion sensor, I have configured a corresponding holder. 
There is a cover to protect the electronics and create a nice look. 
https://github.com/AndreasHuth/ld2410_esp32_radar_motion_detection/tree/master/3D_models

![](https://github.com/AndreasHuth/ld2410_esp32_radar_motion_detection/blob/master/3D_models/Screenshot%202024-10-05%20150717.png)

in real: 

![](https://github.com/AndreasHuth/ld2410_esp32_radar_motion_detection/blob/master/pictures/20241005_145756.jpg)



## How to use
Please download the whole project and open it direktly in Visual Studio Code. 
PlatformIO Extension is necessary.


> [!TIP]
> OneWire project didn't work for me. It was necessary to update file "OneWire_direct_gpio.h" with "#include <soc/gpio_struct.h>"


## further informations
Needed packages:
  paulstoffregen/OneWire @ ^2.3.8
  milesburton/DallasTemperature @ ^3.11.0
  tzapu/WiFiManager @ ^2.0.17
  bblanchon/ArduinoJson @ 5.13.4
  knolleary/PubSubClient @ ^2.8
  ncmreynolds/ld2410 @ ^0.1.4

## Links
na.

## further informations / pictures


![](https://github.com/AndreasHuth/ld2410_esp32_radar_motion_detection/blob/master/pictures/20241005_145814.jpg)

