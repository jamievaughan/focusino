#include <EEPROM.h>
#include <AccelStepper.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

// Pin output configuration.
#define ENABLE_PIN 6
#define STEPPER_PIN 3
#define DIRECTION_PIN 2
#define ONE_WIRE_BUS_PIN 9
#define M1_PIN 5
#define M2_PIN 4
#define LOADING_LED_PIN 13

// Stepper motor configuration.
#define DEFAULT_SPEED 8
#define ACCELERATION_MULTIPLIER 200
#define SPEED_MULTIPLIER 400

// The start addresses in the EEPROM.
#define POSITION_EEPROM_ADDRESS 0
#define SPEED_EEPROM_ADDRESS 4
#define TEMP_COEF_EEPROM_ADDRESS 5
#define TEMP_OFFSET_EEPROM_ADDRESS 6
#define STEP_RESOLUTION_EEPROM_ADDRESS 7
#define HOLDING_EEPROM_ADDRESS 8

// Whether the output pins are enabled/disabled.
bool output;

// The incoming data buffer.
char data[8];
char val;
byte index;

// Variables.
char response[4];
long position, home, target;

byte speed;
bool fullStep, holding;

int raw_temperature,
    temperature,
    temperature_coef,
    temperature_offset;

long lastTemperatureConversion;

// Create the stepper instance.
AccelStepper stepper = AccelStepper(
                         AccelStepper::DRIVER,
                         STEPPER_PIN,
                         DIRECTION_PIN);

// Setup a oneWire instance to communicate with any OneWire devices.
OneWire oneWire(ONE_WIRE_BUS_PIN); 

// Pass our oneWire reference for the Dallas Temperature Sensor. 
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
    writeLongEEPROM(POSITION_EEPROM_ADDRESS, stepper.currentPosition());
    
    setOutputMode(false);
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
