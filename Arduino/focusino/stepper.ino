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
