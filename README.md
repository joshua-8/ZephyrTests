# Learning Zephyr RTOS

# lessons learned
* how to build and upload Zephyr projects to microcontrollers
* starting to understand Zephyr device trees / overlay files
* how to use Zephyr's GPIO, PWM, and I2C drivers
* how to write generic libraries
* how to use Zephyr Threads, Pin Interrupts, and Message Queues

# projects
## [esp32_blink](https://github.com/joshua-8/ZephyrTests/tree/main/esp32_blink)
The simple blink example, but with an overlay file so it runs on an ESP32. I used an ESP32 at first so I could get started while I waited for an nRF5340 board. 
## [esp32_motor](https://github.com/joshua-8/ZephyrTests/tree/main/esp32_motor)
* use PWM to control an HBridge
* overlay file for esp32 that connects pins to the esp32's "ledc" PWM system
* I made a HBridge class for Zephyr (though it isn't made to be portable to other embedded frameworks).
* I used a PCB I had made with an ESP32 and a HBridge https://github.com/RCMgames/RCM-Hardware-V4
## [esp32_motor_ctrl](https://github.com/joshua-8/ZephyrTests/tree/main/esp32_motor_ctrl)
* uses a control loop to control the motor speed based on feedback received from an I2C sensor
* I made a portable version of the [byte-sized-encoder-decoder](https://github.com/rcmgames/bsed) library for the bsed device I made that reads quadrature encoders and sends the data over I2C
* I made a control loop class that uses difference equations so it could be used with controllers other than PID.

https://github.com/user-attachments/assets/348fa874-4575-4c41-a1fd-ec449704fb58

---

## [nRF5340_multithreaded](https://github.com/joshua-8/ZephyrTests/tree/main/nRF3540_multithreaded)
* uses the 4 buttons and 4 leds built into the nRF5340 dev board and uses multiple threads
* Button 3 has a thread that polls the button's state and measures how long it is pressed for. When it is released, it uses a message queue to send the time the led3 thread. 
* Button 4 uses interrupts to measure how long the button is pressed for. When it is released, it uses a message queue to send the time to the led4 thread.
* The led3 and led4 threads blink the leds for the amount of time received from the button threads.
* The main thread directly controls led1 and led2 from button 1 and button 2.

https://github.com/user-attachments/assets/5a677c85-0761-484e-a952-27a77a1f3070

---

# references
* https://www.youtube.com/@thepullupresistor/videos
* https://www.scilab.org/discrete-time-pid-controller-implementation

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

