/**
 * Write a long to an address in the EEPROM.
 */
void writeLongEEPROM(int address, long value) {
  EEPROM.update(address, (value & 0xFF));
  EEPROM.update(address + 1, ((value >> 8) & 0xFF));
  EEPROM.update(address + 2, ((value >> 16) & 0xFF));
  EEPROM.update(address + 3, ((value >> 24) & 0xFF));
}

/**
 * Read a long from an addres in the EEPROM.
 */
long readLongEEPROM(int address) {
  return (((long) EEPROM.read(address) << 0) & 0xFF) +
         (((long) EEPROM.read(address + 1) << 8) & 0xFFFF) +
         (((long) EEPROM.read(address + 2) << 16) & 0xFFFFFF) +
         (((long) EEPROM.read(address + 3) << 24) & 0xFFFFFFFF);
}
