# CS120B---Thermostat-Temperature-Adjustment-System
Embedded home thermostat and temperature design made with Arduino.

This was a project made for my CS120B embedded systems course that outlines the skeleton of a home thermostat with temperature detection and adjustment. It serves as the brains of the system without actually implementing an entire home AC system.

The system is made using an Arduino microcontroller running a C++ script that implements state machines. The microcontroller is connected to LEDS and a miniature LCD screen as outputs, and uses the joystick as user input. The DHT11 module records room temperature and passes it as hardware input into the C++ script ran on the microcontroller. LED lights are activated and give visual information on what the system is doing

The screen is initially off and needs to be turned on by pressing in the joystick. The LCD display shows the current room temperature and humidity, alongside the desired temperature and humidity. A change in either can be selected by moving the joystick up and down to select either to change humidiy or temperature, and then held left or right to decrease or increase desired values. The LEDs will flash to show that either the humidity or temperature would be changing to fit the desired values, and then rest as static lights when that value is found.
