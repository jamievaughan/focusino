#define BAUDRATE 9600

void setup() {
  Serial.begin(BAUDRATE);

  // Read the speed from EEPROM.
  speed = EEPROM.read(SPEED_EEPROM_ADDRESS);

  stepper.setSpeed(speed * SPEED_MULTIPLIER);
  stepper.setMaxSpeed(speed * SPEED_MULTIPLIER);
  stepper.setAcceleration(speed * ACCELERATION_MULTIPLIER);

  // Read the existing position and set the stepper.
  position = readLong(POSITION_EEPROM_ADDRESS);
  stepper.setCurrentPosition(position);

  // Set the output as disabled until we start moving.
  disableOutputs();
}
