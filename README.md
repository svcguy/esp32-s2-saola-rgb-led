# esp32-s2-saola-rgb-led
A simple pair of methods to use the ESP32-S2-SAOLA-1 onboard RGB led

A pair of short and to the point methods to drive the WS2812B led on the ESP32-S2-SAOLA-1 development board.  Simply pass an RBG (or HSV) color to the method and the RMT peripherial is setup, used to send data, then turned off.

## How to use
Add a components folder to your project folder if there isn't already one.  Then git clone this repo to that components folder.  In theory, it should compile and you can then call the methods to set the color of the led
