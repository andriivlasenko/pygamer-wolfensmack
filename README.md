# pygamer-wolfensmack

This is a demo of software raycasting technique for building old-school FPS games.

Base code was taken from here:
<https://github.com/ssloy/tinyraycaster>

It was adapted to work on a Adafruit PyGamer board (120MHz Cortex M4 with 192KB SRAM).

Wall textures taken from here:
<https://www.textures-resource.com/pc_computer/wolf3d/texture/1375/>

Monster textures taken from here:
<https://forum.zdoom.org/viewtopic.php?f=37&t=66132>

Chainsaw texture taken from here:
<https://forum.zdoom.org/viewtopic.php?t=51919>

In order to run it on your PyGamer, you need Arduino IDE and most of the libraries mentioned here:
<https://learn.adafruit.com/adafruit-pygamer/arduino-libraries>

Also you would need a micro-sd card formatted in FAT32 with these files on it in the root:
* monsters24.bmp
* walltext24.bmp
* chainsaw24.bmp

Best setting for compiler optimization is: "Here be dragons (-Ofast -funroll-loops)".
Overclocking the board to 180-200Mhz gives decent fps.


This whole project was thrown together in two days. Code needs a lot of cleanup...

Here's how it looks on the PyGamer:

<a href="http://www.youtube.com/watch?feature=player_embedded&v=Z86SfXGimNw" target="_blank"><img src="http://img.youtube.com/vi/Z86SfXGimNw/0.jpg" 
alt="PyGamer Demo" width="240" height="180" border="10" /></a>





