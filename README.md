# My LD2410 motion sensor
## Brief description
This project is based on the HLK-LD2410 radar sensor module.
The LD2410 is a human presence detecting radar - 24GHz.

Sensor product description (source: https://hlktech.net/index.php?id=988):
LD2410 is a high-sensitivity 24GHz human presence status sensing moduledeveloped by Hi-link. Its working principle is to use FMCW frequency-modulated continuous waves to detect human targets in the set space. Combined with radar signal processing and precise human body sensing algorithms, it realizes high-sensitivity human presence status sensing, and can identify human bodies in motion and stationary states. And auxiliary information such as the distance of the target can be calculated.

The project combines a 
> LD2410 module
> ESP32 C3 Wemos module
> DS18B20 - digital temperature sensor
> LDR - as a light sensor
> Status LED

The motion sensor is supplied with power via the USB port of the ESP32 module.

A special PCB was developed for the electronics (please contact me if you have any questions). The circuit diagram can also be found here:
https://github.com/AndreasHuth/ld2410_esp32_radar_motion_detection/tree/master/schematic

To use the motion sensor, I have configured a corresponding holder. 
There is a cover to protect the electronics and create a nice look. 

https://github.com/AndreasHuth/ld2410_esp32_radar_motion_detection/tree/master/schematic


But there some special topics (at least for me 😊)
- My first project with a self developed PCB (made in China)
- Usage of standard components (easy to get and therefore cheap)
- Design with a standard 18650 Lipo (charging with a USB-C connector)
- 3d printed chassis with a nice design (made by me)
- Usage of an ESP8266 (Wemos D1 mini) with some special features:
        - programming via micro usb interface
        - Wifi access point to control the robot (very nice easy web-interface)
                Please have a look @ this repository 😊

## How to use
Please download the whole project and open it direktly in Visual Studio Code. 
PlatformIO Extension is necessary.

## further informations
Needed packages:
	links2004/WebSockets @ ^2.3.5
	bblanchon/ArduinoJson @ 5.13.3
	pololu/VL53L0X @ ^1.3.0

## Links

https://github.com/miharix/miharix-wifi-rc


## further informations / pictures

final robot :-) 

![](https://github.com/AndreasHuth/SmallESP8266Robot/blob/master/pics/picture1.jpg)

3D model: 

![](https://github.com/AndreasHuth/SmallESP8266Robot/blob/master/3D_files/smallESP8266Robot_3D_pic1.png)

link to the schematic:

https://github.com/AndreasHuth/SmallESP8266Robot/blob/master/schematic/Schematic_SmallESP8266Robot.pdf

and partlist:

https://github.com/AndreasHuth/SmallESP8266Robot/blob/master/partlist/part_overview.txt


