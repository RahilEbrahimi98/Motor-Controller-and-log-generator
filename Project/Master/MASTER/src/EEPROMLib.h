#include <Arduino.h>
#include <SPI.h>
void write(word Waddress, byte Wdata);
byte read(word address);
String eeprom_read(word address);
void eeprom_write(word Waddress, byte len, String Wdata);