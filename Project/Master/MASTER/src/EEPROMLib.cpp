#include "EEPROMLib.h"

void write(word Waddress, byte Wdata)
{
  digitalWrite(53, LOW);
  SPI.transfer(6); //WREN
  digitalWrite(53, HIGH);
  delay(50);
  digitalWrite(53, LOW);
  SPI.transfer(2); //WRITE 
  SPI.transfer((char)(Waddress>>8));
  SPI.transfer((char)(Waddress));
  SPI.transfer(Wdata);
  digitalWrite(53, HIGH);
  delay(100);
}


void eeprom_write(word Waddress, byte len, String Wdata)
{
  char temp [10];
  write(Waddress, len);
  for(int i = 0;i < len ; i++){
    write(Waddress + i + 1, int(Wdata[i]));
  }
  // write(0, int('h'));
  // write(1, int('i'));
  delay(20);
  
}

byte read(word address){
  digitalWrite(53, LOW);
  SPI.transfer(3); //READ
  SPI.transfer((char)(address>>8));
  SPI.transfer((char)(address));
  byte data = SPI.transfer(0xFF);
  digitalWrite(53, HIGH);
  delay(100);
  return data;
}

String eeprom_read(word address){
  int len;
  len = read(address);
  String s = "";
  for(int i = 0; i < len; i++){
    s += char(read(address + i + 1));
  }
  // s += char(read(0));
  // s += char(read(1));
  return s;
}
