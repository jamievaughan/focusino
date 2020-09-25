#include <EEPROM.h>
#include <AccelStepper.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

#include "Moonlite.h"

// General configuration.
#define BAUDRATE 9600

// Pin output configuration.
#define ENABLE_PIN 6
#define STEPPER_PIN 3
#define DIRECTION_PIN 2
#define ONE_WIRE_BUS_PIN 9
#define M1_PIN 5
#define M2_PIN 4

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

// Used to receive and respond to Moonlite protocol commands.
Moonlite moonlite = Moonlite();

// Create the stepper instance.
AccelStepper stepper = AccelStepper(
                         AccelStepper::DRIVER,
                         STEPPER_PIN,
                         DIRECTION_PIN);

// Setup a oneWire instance to communicate with any OneWire devices.
OneWire oneWire(ONE_WIRE_BUS_PIN); 

// Pass our oneWire reference for the Dallas Temperature Sensor. 
DallasTemperature sensors(&oneWire);

// Store the current moonlite command
MoonliteCommand command;

// General variables.
bool fullStep = false,
     running = false;

byte speed;

int raw_temperature,
    temperature,
    temperature_coef,
    temperature_offset;

long lastTemperatureConversion;

long position, home, target;

/*
 * The sketch setup.
 */
void setup()
{
  sensors.begin();
  
  moonlite.init(BAUDRATE);
  
  // Enable the stepper motor.
  stepper.setEnablePin(ENABLE_PIN);
  
  // Inverse required for the TMC2208.
  stepper.setPinsInverted(false, false, true);
  
  // Read the speed from EEPROM.
  speed = EEPROM.read(SPEED_EEPROM_ADDRESS);
  
  stepper.setSpeed(speed * SPEED_MULTIPLIER);
  stepper.setMaxSpeed(speed * SPEED_MULTIPLIER);
  stepper.setAcceleration(speed * ACCELERATION_MULTIPLIER);
  
  // Read the temperature coefficient from EEPROM.
  temperature_coef = EEPROM.read(TEMP_COEF_EEPROM_ADDRESS);
  
  // Read the temperature offset from EEPROM.
  temperature_offset = EEPROM.read(TEMP_OFFSET_EEPROM_ADDRESS);
  
  // Read the existing position and set the stepper.
  position = readLongFromEEPROM(POSITION_EEPROM_ADDRESS);
  stepper.setCurrentPosition(position);
  
  // Set the home to the startup position.
  home = position;
  
  // Read and set the step resolution from EEPROM.
  setResolution(EEPROM.read(STEP_RESOLUTION_EEPROM_ADDRESS) > 0
    ? true
    : false);
  
  stepper.enableOutputs();
}

/*
 * The sketch main loop.
 */
void loop()
{
  // Run a incremental step of the stepper motor
  // every loop iteration if we're currently moving
  // to a new target position.
  if (stepper.isRunning() && running)
  {      
    stepper.run();
  }
  else if (stepper.distanceToGo() == 0 && running)
  {
    position = stepper.currentPosition();
    
    // Write the stepper position to the EEPROM.
    writeLongIntoEEPROM(POSITION_EEPROM_ADDRESS, position);

    running = false;
  }
  
  // Loop the moonlite protocol monitor.
  moonlite.loop();

  // Handle any incoming Moonlite commands.
  handleIncomingCommand();
}

void handleIncomingCommand()
{  
  // Handle the incoming moonlite protocol command.
  if (!moonlite.hasReceivedCommand())
  {
    return;
  }

  command = moonlite.getCommand();
 
  switch (command.id)
  {
    case MOONLITE_GET_FIRMWARE_VERSION:
      moonlite.respond(1, DEC);
      break;

    case MOONLITE_GET_BACKLIGHT:
      moonlite.respond(0, DEC);
      break;

    case MOONLITE_TEMPERATURE_CONVERSION:
      if (running)
      {
        return;
      }
      
      // The requesting and conversion of the temperature is an
      // expensive process and must be limited.
      if (millis() - lastTemperatureConversion < 5000) {
        return;
      }
      
      sensors.requestTemperatures();
      
      // This is a slow process.
      raw_temperature = sensors.getTempCByIndex(0);
      
      // Divide by 2 as each increment is half a degree offset.
      temperature = ((raw_temperature * temperature_coef) + temperature_offset) * 2.0;      
      
      lastTemperatureConversion = millis();
      break;

    case MOONLITE_GET_TEMPERATURE_COEFFICIENT:
      moonlite.respond(temperature_coef, DEC);
      break;

    case MOONLITE_SET_TEMPERATURE_COEFFICIENT:
      temperature_coef = command.parameter;
      
      EEPROM.update(TEMP_COEF_EEPROM_ADDRESS, temperature_coef);
      break;

    case MOONLITE_GET_TEMPERATURE_CALIBRATION_OFFSET:
      moonlite.respond(temperature_offset, HEX);
      break;

    case MOONLITE_SET_TEMPERATURE_CALIBRATION_OFFSET:
      temperature_offset = command.parameter;
      
      EEPROM.update(TEMP_OFFSET_EEPROM_ADDRESS, temperature_offset);
      break;

    case MOONLITE_GET_TEMPERATURE:
      moonlite.respond(temperature, HEX);
      break;

    case MOONLITE_GET_STEPPER_SPEED:
      moonlite.respond(speed, DEC);
      break;

    case MOONLITE_SET_STEPPER_SPEED:
      speed = command.parameter;
      
      stepper.setSpeed(speed * SPEED_MULTIPLIER);
      stepper.setMaxSpeed(speed * SPEED_MULTIPLIER);
      stepper.setAcceleration(speed * ACCELERATION_MULTIPLIER);
      
      EEPROM.update(SPEED_EEPROM_ADDRESS, speed);
      break;

    case MOONLITE_GET_CURRENT_POSITION:
      moonlite.respond(stepper.currentPosition(), HEX);
     break;

    case MOONLITE_GET_TARGET_POSITION:
      moonlite.respond(target, HEX);
      break;
    
    case MOONLITE_SET_TARGET_POSITION:
      if (running)
      {
        return;
      }
      
      target = command.parameter;
      break;

    case MOONLITE_SET_CURRENT_POSITION:
      if (running)
      {
        return;
      }

      position = command.parameter;
      
      stepper.setCurrentPosition(position);
      
      writeLongIntoEEPROM(POSITION_EEPROM_ADDRESS, position);
      break;

    case MOONLITE_GOTO_TARGET_POSITION:        
      stepper.moveTo(target);
      
      if (stepper.distanceToGo() == 0)
      {
        return;
      }

      running = true;
      break;

    case MOONLITE_STOP_MOVEMENT:
      stepper.stop();
    break;

    case MOONLITE_GET_RUNNING_STATE:
      moonlite.respond(
        running
          ? MOONLITE_RUNNING
          : MOONLITE_NOT_RUNNING,
          DEC);
      break;

    case MOONLITE_SET_FULL_STEP_MODE:
    case MOONLITE_SET_HALF_STEP_MODE:
    {
      bool fullStepMode = command.id == MOONLITE_SET_FULL_STEP_MODE;

      setResolution(fullStepMode);
      
      EEPROM.update(
        STEP_RESOLUTION_EEPROM_ADDRESS,
        fullStepMode ? 1 : 0);
    }
    break;

    case MOONLITE_GET_STEP_MODE:
      moonlite.respond(
        fullStep
          ? MOONLITE_STEPPER_FULL_STEP
          : MOONLITE_STEPPER_HALF_STEP,
          DEC);
      break;
  }
}

void setResolution(bool mode)
{  
  digitalWrite(M1_PIN, mode ? HIGH : LOW);
  digitalWrite(M2_PIN, mode ? HIGH : LOW);
  
  fullStep = mode;
}

void writeLongIntoEEPROM(int address, long value)
{
  EEPROM.update(address, (value & 0xFF));
  EEPROM.update(address + 1, ((value >> 8) & 0xFF));
  EEPROM.update(address + 2, ((value >> 16) & 0xFF));
  EEPROM.update(address + 3, ((value >> 24) & 0xFF));
}

long readLongFromEEPROM(int address)
{
  return (((long) EEPROM.read(address) << 0) & 0xFF) +
         (((long) EEPROM.read(address + 1) << 8) & 0xFFFF) +
         (((long) EEPROM.read(address + 2) << 16) & 0xFFFFFF) +
         (((long) EEPROM.read(address + 3) << 24) & 0xFFFFFFFF);
}
