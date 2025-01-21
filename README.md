# setup

on a computer with zephyr installed, in Windows CMD running as administrator, cd to an empty folder then run

python -m venv %userprofile%\zephyrproject\.venv

%userprofile%\zephyrproject\.venv\Scripts\activate.bat

west init

west update

(if needed)
git config --global --add safe.directory ZephyrTests




# building
in CMD

%userprofile%\zephyrproject\.venv\Scripts\activate.bat

west build -p always

west flash

