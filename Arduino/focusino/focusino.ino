#include <EEPROM.h>
#include <AccelStepper.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

// Pin output configuration
#define STEPPER_PIN 3
#define DIRECTION_PIN 2
#define ONE_WIRE_BUS_PIN 5
#define M2_PIN 7

// Motor configuration
#define DEFAULT_SPEED 8
#define ACCELERATION_MULTIPLIER 200
#define SPEED_MULTIPLIER 400

// The start address in the EEPROM for the position.
#define POSITION_EEPROM_ADDRESS 0

// The start address in the EEPROM for the speed.
#define SPEED_EEPROM_ADDRESS 4

// The start address in the EEPROM for the temperature coeffcient.
#define TEMP_COEF_EEPROM_ADDRESS 5

// The start address in the EEPROM for the temperature offset.
#define TEMP_OFFSET_EEPROM_ADDRESS 6

// The start address in the EEPROM for the step resolution.
#define STEP_RESOLUTION_EEPROM_ADDRESS 7

// Whether the output pins are enabled/disabled.
bool output;

// The incoming data buffer.
char data[8];
char val;
byte index;

// Variables
char response[4];
long position, target;

byte speed;
bool fullStep;

int raw_temperature,
    temperature,
    temperature_coef,
    temperature_offset;

// Create the stepper instance.
AccelStepper stepper = AccelStepper(
                         AccelStepper::DRIVER,
                         STEPPER_PIN,
                         DIRECTION_PIN);

// Setup a oneWire instance to communicate with any OneWire devices.
OneWire oneWire(ONE_WIRE_BUS_PIN); 

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void loop() {
  // Run a incremental step of the stepper motor
  // every loop iteration if we're currently moving
  // to a new target position.
  if (stepper.isRunning() && output) {      
    stepper.run();
  }
  else if (stepper.distanceToGo() == 0 && output) {
    // Write the stepper position to the EEPROM.
    writeLong(POSITION_EEPROM_ADDRESS, stepper.currentPosition());
    
    disableOutputs();
  }
  
  // Listen for any incoming serial data.
  if (Serial.available()) {
    // Rather than lock up the stepping of the motor
    // and cause judder we read this one byte per iteration.
    val = Serial.read();

    data[index++] = val;

    // We've received a command terminator
    // so its time to process the command.
    if (val == '#') {      
      handleIncomingData();
      
      index = 0;
    }
  }
}
