#define BAUDRATE 9600

void setup() {
  Serial.begin(BAUDRATE);
  
  sensors.begin(); 

  // Read the speed from EEPROM.
  speed = EEPROM.read(SPEED_EEPROM_ADDRESS);

  // Read and set the step resolution from EEPROM.
  setResolution(EEPROM.read(STEP_RESOLUTION_EEPROM_ADDRESS) == 1
    ? true
    : false);

  // Read the temperature coefficient from EEPROM.
  temperature_coef = sign(EEPROM.read(TEMP_COEF_EEPROM_ADDRESS), 8);
  
  // Read the temperature offset from EEPROM.
  temperature_offset = sign(EEPROM.read(TEMP_OFFSET_EEPROM_ADDRESS), 8);

  stepper.setEnablePin(ENABLE_PIN);
  
  // Required for the TMC2208.
  stepper.setPinsInverted(false, false, true);
  
  stepper.setSpeed(speed * SPEED_MULTIPLIER);
  stepper.setMaxSpeed(speed * SPEED_MULTIPLIER);
  stepper.setAcceleration(speed * ACCELERATION_MULTIPLIER);

  // Read the existing position and set the stepper.
  position = readLongEEPROM(POSITION_EEPROM_ADDRESS);
  stepper.setCurrentPosition(position);

  // Set the output as disabled until we start moving.
  disableOutputs();
}
