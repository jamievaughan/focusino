# Focusino
A custom firmware solution for a MoonLite protocol compatible Arduino telescope focuser.

## Features
* Non-blocking command pipeline
* Stepper motor resolution (full/half step)
* Speed control /w acceleration/deacceleration
* Temperature /w offset and coefficient calibration
* Efficient output enable/disabling when motor is not active

## MoonLite Documentation
https://indilib.org/media/kunena/attachments/1/HighResSteppermotor107.pdf

## TODO
* Position homing
* Configurable output enable/disable to maintain holding torque (required for TMC2208)

## Hardware
* Arduino Nano
* TMC2208 Silent Driver (Also works with DRV8825 / A4988)
* NEMA 17 Stepper Motor
* DS18B20 Temperature Sensor

## Contributing
All contributions are very welcome!
