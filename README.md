# CactusCon 8 badge

This year's CactusCon electronic badge features a number of sensors that can be used to collect "bio" data.

## Badge kit

![alt text](https://github.com/cactuscon/cactuscon8/raw/master/img/badge_kit_sm.png "badge kit components")

### Included components

The badge kit include the following items:

- PCB
- 3.7V 603450 1200mAh LiPo battery 
- [TTGO](http://www.lilygo.cn/claprod_view.aspx?TypeId=21&Id=1219) T-Display ESP32[dev board](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)
- [GY-521](https://playground.arduino.cc/Main/MPU-6050/) [MPU-6050](https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/) 6-axis gyroscope/accelerometer
- [MAX30102](https://www.maximintegrated.com/en/products/interface/sensor-interface/MAX30102.html) high-sensitivity pulse oximeter and heart rate sensor
- Headers x 2
- Lanyard
- Adhesive foam pad

### Optional Components*

- Thermal IR sensor
- Piezo speaker
- MQ gas sensor 
 - Transistor
 - 1k ohm SMD resistor
- 0.96" OLED display

* More details to come

## Assembly

Assembly instructions can be found in this document.  To begin assembling the included components, skip straight to step C.

## Installing firmware

The easiest way to install the firmware on the T-Display ESP32 dev board is to use [PlatformIO](https://platformio.org/).  

### Installing Platform IO

1. Install [VS Code](https://code.visualstudio.com/download) if you don't already have it installed
2. Install the PlatformIO plugin
![alt text](https://github.com/cactuscon/cactuscon8/raw/master/img/install_platformio.png "Install PlatformIO")
3. Clone this [git repo](https://github.com/cactuscon/cactuscon8)
4. Import project
![alt text](https://github.com/cactuscon/cactuscon8/raw/master/img/open_project.png "Open project")
5. Push firmware 
![alt text](https://github.com/cactuscon/cactuscon8/raw/master/img/push_firmware.png "Push firmware")

## PCB design

[Kicad](https://kicad-pcb.org/) was used to design the this year's badge and you can find the design files [here](https://github.com/cactuscon/cactuscon8/raw/master/doc/2019CactusCon_PCB.zip).

## What's next?
What are you going to build with your badge? Share your hacks with us at 'badge at cactuscon com' or on Twitter (@CactusCon)!  PRs welcome.
