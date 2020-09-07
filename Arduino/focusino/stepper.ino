void setHolding(bool enabled, bool updateEeprom) {
  if (holding == enabled) {
    return;
  }
  
  holding = enabled;

  if (enabled) {
    stepper.enableOutputs();
  }
  else {    
    stepper.disableOutputs();
  }

  if (updateEeprom) {
    EEPROM.update(
      HOLDING_EEPROM_ADDRESS,
      enabled ? 1 : 0);
  }
}
/**
* Change the output mode including the output mode
* of the stepper.
*/
void setOutputMode(bool enabled) {
  if (output == enabled) {
    return;
  }

  if(!holding) {
    if (enabled) {
      stepper.enableOutputs();
    }
    else {    
      stepper.disableOutputs();
    }
  }
  
  digitalWrite(LOADING_LED_PIN, enabled ? HIGH : LOW);
  
  output = enabled;
}

/**
 * Set the stepper motor resolution.
 */
void setResolution(bool full, bool updateEeprom) {
  if (fullStep == full) {
    return;
  }
  
  fullStep = full;
  
  digitalWrite(M1_PIN, full ? HIGH : LOW);
  digitalWrite(M2_PIN, full ? HIGH : LOW);

  if (updateEeprom) {
    EEPROM.update(
      STEP_RESOLUTION_EEPROM_ADDRESS,
      full ? 1 : 0);
  }
}
