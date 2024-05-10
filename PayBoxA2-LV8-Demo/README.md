# PayBoxA2-LVWatch
LVGL 8 Demo for PayBox A2 With Arduino

#### Screenshot
![sample](https://cdn.jsdelivr.net/gh/zhuhai-esp/PayBoxA2-LVWatch/misc/screen.gif)

#### Ported and features
* LovyanGFX
* LVGL
* WiFi SmartConfig
* ArduinoOTA
* NTP Client

#### How To Use
Download Visual Studio Code, Open This Folder, Wait a while and ready to go!

#### Use OTA
Open platformio.ini and edit:
```
upload_protocol = espota
upload_port = 192.168.2.246
```
upload_port should be the IP address of the ESP32 Module and ready to go!

