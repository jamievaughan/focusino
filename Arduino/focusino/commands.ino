/**
 * Handle incoming command data.
 */
void handleIncomingData() {  
  if (strncmp(&data[1], "C", 1) == 0) { // Initiaze a temperature conversion.
  }
  else
  if (strncmp(&data[1], "GB", 2) == 0) { // Get backlight value.
    Serial.print("00#");
  }
  else
  if (strncmp(&data[1], "GV", 2) == 0) { // Get firmware version.
    Serial.print("10#");
  }
  else
  if (strncmp(&data[1], "GP", 2) == 0) { // Get position.
    position = stepper.currentPosition();
    ltoa(position, response, 16);

    Serial.print(response);
    Serial.print('#');
  }
  else
  if (strncmp(&data[1], "GN", 2) == 0) { // Get target position.
    target = stepper.targetPosition();
    ltoa(target, response, 16);

    Serial.print(response);
    Serial.print('#');
  }
  else
  if (strncmp(&data[1], "GT", 2) == 0) { // Get temperature.
      Serial.print("20#");
  }
  else
  if (strncmp(&data[1], "GC", 2) == 0) { // Get temperature coefficient.
      Serial.print("02#");
  }
  else
  if (strncmp(&data[1], "GD", 2) == 0) { // Get motor speed.
    // Speeds - 02, 04, 08, 10, 20
    Serial.print(speed, HEX);
    Serial.print("#");
  }
  else
  if (strncmp(&data[1], "SD", 2) == 0) { // Set motor speed.
    speed = strtol(&data[3], NULL, 16);
    
    stepper.setSpeed(speed * SPEED_MULTIPLIER);
    stepper.setMaxSpeed(speed * SPEED_MULTIPLIER);
    stepper.setAcceleration(speed * ACCELERATION_MULTIPLIER);
    
    EEPROM.update(SPEED_EEPROM_ADDRESS, speed);
  }
  else
  if (strncmp(&data[1], "GH", 2) == 0) { // Get half-step state.
    Serial.print("FF#"); // FF - Half-step / 00 - Full-step
  }
  else
  if (strncmp(&data[1], "GI", 2) == 0) { // Get running state.
    Serial.print(output ? "01#" : "00#");
  }
  else
  if (strncmp(&data[1], "PH", 2) == 0) { // Home the motor.
    if (output) {
      return;
    }

    // TODO: Home the motor.
  }
  else
  if (strncmp(&data[1], "SP", 2) == 0) { // Set to current position.
    if (output) {
      return;
    }
    
    position = strtol(&data[3], NULL, 16);

    stepper.setCurrentPosition(position);
    writeLong(POSITION_EEPROM_ADDRESS, position);
  }
  else
  if (strncmp(&data[1], "SN", 2) == 0) { // Set the new target position.
    if (output) {
      return;
    }
    
    position = strtol(&data[3], NULL, 16);
    
    stepper.moveTo(position);
  }
  else
  if (strncmp(&data[1], "FG", 2) == 0) { // Start the motor running.
    if (stepper.distanceToGo() == 0) {
      return;
    }
    
    enableOutputs();
  }
  else
  if (strncmp(&data[1], "FQ", 2) == 0) { // Stop the motor from running.
    disableOutputs();
  }
}
