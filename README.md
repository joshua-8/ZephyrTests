
# projects
## [esp32_blink](https://github.com/joshua-8/ZephyrTests/tree/main/esp32_blink)
The simple blink example, but with an overlay file so it runs on an ESP32. I used an ESP32 while I waited for an nRF5340 board. 
## [esp32_motor](https://github.com/joshua-8/ZephyrTests/tree/main/esp32_motor)
* use PWM to control an HBridge
* overlay file for esp32 that connects pins to the esp32's "ledc" PWM system
* I made a HBridge class for Zephyr (though it isn't made to be portable to other embedded frameworks).
* using a PCB I designed https://github.com/RCMgames/RCM-Hardware-V4

# references
* https://www.youtube.com/@thepullupresistor/videos

---

## setup

on a computer with zephyr installed, in Windows CMD running as administrator, cd to an empty folder then run

python -m venv %userprofile%\zephyrproject\.venv

%userprofile%\zephyrproject\.venv\Scripts\activate.bat

west init

west update

(if needed)
git config --global --add safe.directory ZephyrTests




## building
in CMD

%userprofile%\zephyrproject\.venv\Scripts\activate.bat

west build -p always

west flash

