/**
 * Disable the stepper motor output pins
 * by setting their state to LOW.
 */
void disableOutputs() {
  if (!output) {
    return;
  }
  
  stepper.disableOutputs();
  digitalWrite(13, LOW);
  
  output = false;
}

/**
* Enable the stepper motor pins
* by setting their state to HIGH.
*/
void enableOutputs() {
  if (output) {
    return;
  }
  
  stepper.enableOutputs();
  digitalWrite(13, HIGH);
  
  output = true;
}

/**
 * Set the stepper motor resolution.
 */
void setResolution(bool full) {
    fullStep = full;

    digitalWrite(M2_PIN, full ? LOW : HIGH);
    
    EEPROM.update(
      STEP_RESOLUTION_EEPROM_ADDRESS,
      full ? 1 : 0);  
}
