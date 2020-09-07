/**
 * Handle incoming command data.
 * 
 * Example - :GV# <- Get the firmware version.
 */
void handleIncomingData() {
  if (strncmp(&data[1], "C", 1) == 0) { // Initiate temperature conversion.
    if (output) {
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
    temperature = (raw_temperature * temperature_coef) + (temperature_offset / 2.0);

    lastTemperatureConversion = millis();
  }
  else if (strncmp(&data[1], "GB", 2) == 0) { // Get backlight value.
    writeResponse("00");
  }
  else if (strncmp(&data[1], "GV", 2) == 0) { // Get firmware version.
    writeResponse("10");
  }
  else if (strncmp(&data[1], "GP", 2) == 0) { // Get position.
    position = stepper.currentPosition();
    sprintf(response, "%04X", position);

    writeResponse(response);
  }
  else if (strncmp(&data[1], "GN", 2) == 0) { // Get target position.
    target = stepper.targetPosition();
    sprintf(response, "%04X", target);

    writeResponse(response);
  }
  else if (strncmp(&data[1], "GT", 2) == 0) { // Get temperature.    
    sprintf(response, "%04X", temperature);

    writeResponse(response);
  }
  else if (strncmp(&data[1], "GC", 2) == 0) { // Get temperature coefficient.
    sprintf(response, "%02X", temperature_coef);
    
    writeResponse(response);
  }
  else if (strncmp(&data[1], "GO", 2) == 0) { // Get temperature offset.
    sprintf(response, "%04X", temperature_offset);
    
    writeResponse(response);
  }
  else if (strncmp(&data[1], "GD", 2) == 0) { // Get motor speed.
    // Speeds - 02, 04, 08, 10, 20
    sprintf(response, "%02X", speed);
    
    writeResponse(response);
  }
  else if (strncmp(&data[1], "SD", 2) == 0) { // Set motor speed.
    speed = strtol(&data[3], NULL, HEX);
    
    stepper.setSpeed(speed * SPEED_MULTIPLIER);
    stepper.setMaxSpeed(speed * SPEED_MULTIPLIER);
    stepper.setAcceleration(speed * ACCELERATION_MULTIPLIER);
    
    EEPROM.update(SPEED_EEPROM_ADDRESS, speed);
  }
  else if (strncmp(&data[1], "GH", 2) == 0) { // Get half-step state.
    writeResponse(fullStep ? "00" : "FF"); // FF - Half-step / 00 - Full-step
  }
  else if (strncmp(&data[1], "GI", 2) == 0) { // Get running state.
    writeResponse(output ? "01" : "00");
  }
  else if (strncmp(&data[1], "PH", 2) == 0) { // Home the motor.
    if (output) {
      return;
    }

    if (position == home) {
      return;
    }

    stepper.moveTo(home);

    enableOutputs();
  }
  else if (strncmp(&data[1], "SC", 2) == 0) { // Set temperature coefficient.
    temperature_coef = sign(strtol(&data[3], NULL, HEX), 8);

    EEPROM.update(TEMP_COEF_EEPROM_ADDRESS, temperature_coef);
  }
  else if (strncmp(&data[1], "PO", 2) == 0) { // Set the temperature calibration offset.
    temperature_offset = sign(strtol(&data[3], NULL, HEX), 8);
 
    EEPROM.update(TEMP_OFFSET_EEPROM_ADDRESS, temperature_offset);
  }
  else if (strncmp(&data[1], "SP", 2) == 0) { // Set to current position.
    if (output) {
      return;
    }
    
    position = strtol(&data[3], NULL, HEX);

    stepper.setCurrentPosition(position);
    writeLongEEPROM(POSITION_EEPROM_ADDRESS, position);
  }
  else if (strncmp(&data[1], "SN", 2) == 0) { // Set the new target position.
    if (output) {
      return;
    }
    
    position = strtol(&data[3], NULL, HEX);
    
    stepper.moveTo(position);
  }
  else if (strncmp(&data[1], "FG", 2) == 0) { // Start the motor running.
    if (stepper.distanceToGo() == 0) {
      return;
    }
    
    enableOutputs();
  }
  else if (strncmp(&data[1], "FQ", 2) == 0) { // Stop the motor from running.
    disableOutputs();
  }
  else if (strncmp(&data[1], "SF", 2) == 0) { // Set full-step mode.
    setResolution(true);
  }
  else if (strncmp(&data[1], "SH", 2) == 0) { // Set half-step mode.
    setResolution(false);
  }
}
