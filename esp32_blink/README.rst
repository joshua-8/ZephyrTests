# building

in CMD

%userprofile%\zephyrproject\.venv\Scripts\activate.bat

cd esp32_blink

west build -p always

west flash

