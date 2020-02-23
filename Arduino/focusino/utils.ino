/**
 * Signed a value.
 */
long sign(long value, byte bits) {
  // Convert to a signed value.
  if (value >= (1 << (bits - 1)))
      value -= (1 << bits);

  return value;
}
