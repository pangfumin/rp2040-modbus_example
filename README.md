# Modbus RTU Slave library for Raspberry Pi Pico (RP2040) based on the C++ SDK
This repository contain **working** example Modbus RTU Slave implementation for rp2040 (Raspberry Pi Pico) with C++ SDK

This is a fork from https://github.com/gleboss-redfab/rp2040-modbus_example<br>
From the fork base class modbusManager, modbus.hpp,  I create a subclass ModbusPico.<br>
This way I just need to modify ModbusPico class to add function.<br>

On the current example the modbus layout has 8 outputs, 8 inputs, one wire DS18B20 line and analog ADC inputs.<br>

<img src="PicoModbusLayout.jpg" height=480>


## Update Feb 2024
- add two bme280 sensors on i2c gpio10 and gpio11<br>
  input register address<br>
  2200   bit0=bme280 sensor 1, bit1=bme280 sensor 2.  0=Invalid 1=Valid<br>
  2201..2202   sensor 1  signed   32bits temperature * 100 in celsius<br>
  2203..2204   sensor 1  unsigned 32bits Humidity * 1024   in %<br>
  2205..2206   sensor 1  signed   32bits Pressure * 100    in HPa<br>
  2207..2208   sensor 1  signed   32bits temperature * 100 in celsius<br>
  2209..2210   sensor 1  unsigned 32bits Humidity * 1024   in %<br>
  2211..2212   sensor 1  signed   32bits Pressure * 100    in HPa<br>
  <br>
  Check readBME280.py for info<br>


## Features:
- Modbus RTU Slave
- **RS-485** support
- C++ style class 
- Registers layout
- Separate setup read-only and read-write registers
- UART 0 and 1 support
- it works out of box and tested


## How to use the example:
- Tune VS Code for Raspberry Pico development ([instruction](https://www.youtube.com/watch?v=B5rQSoOmR5w))
- You can use 2nd Raspberry Pico as a bootloader and debuger ([instruction](https://www.youtube.com/watch?v=jnC5LrTx470))
- Modify launch.json and setting.json files in .vscode according to your environment
- Start the debugging session 
- Use Modbus Master emulator for connection (ModBus Poll for example)
- Change UART parameters in main.cpp and registers layout in modbus.cpp for your purpose
- Change stored data inside Modbus Manager class
- Change Switch-case optins according to your data and registers in *mb_read_holding_register()* and *mb_write_single_register()*
- 
## How to compile using a raspberry Pi connected to the swd
-  Follow the installation step from the PDF  "Raspberry Pi Pico c/C++ SDK". https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html
-  clone the github.  <b>git clone https://github.com/danjperron/rp2040-modbus_example.git</b><br>
-  Enter the  rp2040-modbus_example folder.   <b>cd rp2040-modbus_example</b><br>
-  Create the build folder.   <b>mkdir build</b><br>
-  Enter the build folder.     <b>cd build</b><br>
-  Create the cmake environnement.     <b>cmake ..</b>
-  Compile.       <b>make</b><br>
-  To transfer use openocd.  https://iosoft.blog/2019/01/28/raspberry-pi-openocd <br>

