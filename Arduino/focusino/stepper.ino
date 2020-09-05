/**
 * Disable the stepper motor output pins
 * by setting their state to LOW.
 */
void disableOutputs() {
  if (!output) {
    return;
  }
  
  stepper.disableOutputs();
  digitalWrite(LOADING_LED_PIN, LOW);
  
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
  digitalWrite(LOADING_LED_PIN, HIGH);
  
  output = true;
}

/**
 * Set the stepper motor resolution.
 */
void setResolution(bool full) {
  if (fullStep == full) {
    return;
  }
  
  fullStep = full;
  
  digitalWrite(M1_PIN, full ? HIGH : LOW);
  digitalWrite(M2_PIN, full ? HIGH : LOW);
  
  EEPROM.update(
    STEP_RESOLUTION_EEPROM_ADDRESS,
    full ? 1 : 0);  
}
