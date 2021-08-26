#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EEPROMLib.h"  //for external eeprom
#include <EEPROM.h>  //for internal eeprom


LiquidCrystal_I2C lcd(0x20, 32, 2);
String error_code = "";

volatile int danger_flag = 0;
int Sec, Min, Hour, Day, Date, Month, Year;
int adc = 0;
int prev_adc = 0;
int flag = 0;
int duty;
int pc = 0;

int bcdToDec(int bcd){
  int ten = bcd >> 4;
  int one = bcd & 15;
  return 10*ten + one;
}

void Date_time(){
  Wire.beginTransmission(0x68);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7);

  if(Wire.available()){
    Sec = Wire.read();
    Min = Wire.read();
    Hour = Wire.read();
    Day = Wire.read();
    Date = Wire.read();
    Month = Wire.read();
    Year = Wire.read();
  }
  Wire.endTransmission();

  lcd.setCursor(0,0);
  lcd.print("Date: ");
  if (Year < 10){
    lcd.print(0);
  }
  lcd.print(bcdToDec(Year));
  lcd.print("/ ");
  if (Month < 10){
    lcd.print(0);
  }
  lcd.print(bcdToDec(Month));
  lcd.print("/ ");
  if (Date < 10){
    lcd.print(0);
  }
  lcd.print(bcdToDec(Date));
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  if (Hour < 10){
    lcd.print(0);
  }
  lcd.print(bcdToDec(Hour));
  lcd.print(": ");
  if (Min < 10){
    lcd.print(0);
  }
  lcd.print(bcdToDec(Min));
  lcd.print(": ");
  if (Sec < 10){
    lcd.print(0);
  }
  lcd.print(bcdToDec(Sec));
  //delay(500);
}

void external_isr(){
  if (digitalRead(2) == HIGH){  //critical
    danger_flag = 1;
    Serial.println("critical state found");
  }
  else if (digitalRead(2) == LOW){
    danger_flag = 0;
    Serial.println("critical state finished");
  }
  
  Serial.println("external interrupt");
}

void send_duty(){
    duty = map(adc, 0, 1023, 0, 255);
    Serial3.write(duty);
    lcd.clear();
    lcd.print("new duty cycle: ");
    lcd.print(map(duty, 0, 255, 0, 100));
    lcd.print(" %");
    delay(100);   // im using 100ms instead of 5000ms to be faster
    lcd.clear();
    Date_time();
}

int get_pc(){   //get pc(first empty cell)
  int msb = EEPROM.read(0);
  int lsb = EEPROM.read(1);
  msb = msb << 8;
  return msb + lsb; 
}

void clear_eeprom(){
    EEPROM.write(1, 0);
    EEPROM.write(0, 0);
}

void update_pc(int new_address){  //write new pc to internal eeprom
    clear_eeprom();
    EEPROM.write(1, new_address);
    int msb = new_address >> 8;
    EEPROM.write(0, msb);
}

void error(int error_number){  //generate proper error and write it to eeprom
  error_code = "EC";
  char temp [3];

  //error code
  itoa(error_number, temp, 10);
  error_code = error_code + temp;
  error_code = error_code + '_';
  
  //duty
  int percent = map(duty, 0, 255, 0, 100);
  itoa(percent, temp, 10);
  error_code = error_code + temp;
  error_code = error_code + '_';


  Wire.beginTransmission(0x68);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7);

  if(Wire.available()){
    Sec = Wire.read();
    Min = Wire.read();
    Hour = Wire.read();
    Day = Wire.read();
    Date = Wire.read();
    Month = Wire.read();
    Year = Wire.read();
  }
  Wire.endTransmission();

  //date
  if(bcdToDec(Date) < 10){
    error_code = error_code + '0';
  }
  itoa(bcdToDec(Date), temp, 10);
  error_code = error_code + temp;

  if(bcdToDec(Month) < 10){
    error_code = error_code + '0';
  }
  itoa(bcdToDec(Month), temp, 10);
  error_code = error_code + temp;

  if(bcdToDec(Year) < 10){
    error_code = error_code + '0';
  }
  itoa(bcdToDec(Year), temp, 10);
  error_code = error_code + temp;
  error_code = error_code + '_';

  //time
  if(bcdToDec(Hour) < 10){
    error_code = error_code + '0';
  }
  itoa(bcdToDec(Hour), temp, 10);
  error_code = error_code + temp;

  if(bcdToDec(Min) < 10){
    error_code = error_code + '0';
  }
  itoa(bcdToDec(Min), temp, 10);
  error_code = error_code + temp;

  Serial.println("error code generated:");
  Serial.println(error_code);
  //write the error to eeprom 
  pc = get_pc(); //first byte of the last error
  Serial.println("current pc:");
  Serial.println(pc);
  //Serial.println(read(0));
  if (pc == 0 && read(0) == 255 ){  //eeprom is empty
    Serial.println("writing to eeprom...");
    eeprom_write(0, error_code.length(), error_code);
    pc = 0;
  }
  else{
    String last_error = eeprom_read(pc);
    int first_empty = pc + last_error.length() + 1;
    Serial.println("first empty location:");
    Serial.println(first_empty);
    Serial.println("writing to eeprom...");
    eeprom_write(first_empty, error_code.length(), error_code);
    pc = first_empty; //first byte of the new error
  }
  
  update_pc(pc);
  Serial.println("write state done");
  
}

void danger(){
    //String new_error = error(101);
    lcd.clear();
    lcd.print("Danger");
    Serial3.write(0); //send 0 duty cycle to stop the motor
    error(101);
  
    while(danger_flag == 1){
      lcd.clear();
      delay(100);
      lcd.print("danger");
      delay(100);
    }
    
}

bool check_error_code(String code){ //return true if 303
  Serial.println(code[4]);
  if (code[4] == '3'){
    Serial.println("its error 303");
    return true;
  }
  else {
    Serial.println("error not 303");
    return false;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(A0, INPUT);

  attachInterrupt(digitalPinToInterrupt(2), external_isr, CHANGE);

  lcd.init(); 
  lcd.backlight();   

  Serial3.begin(9600);
  adc = analogRead(A0);
  prev_adc = adc;

  Serial.begin(9600);
  SPI.begin();
  
  pc = get_pc();
  Serial.println("pc is:");
  Serial.println(pc);
  //error(303);   //for test
  if (pc == 0 & read(0) == 255){
    lcd.print("its the first run!no last state");
    error(303); //write error 303 to eeprom
  }
  else{
    Serial.println("reading last state...");
    String last_state = eeprom_read(pc);
    lcd.clear();
    lcd.print(last_state);
    if(check_error_code(last_state)){ // its 303 error
      error(303); //write error 303 to eeprom
    }
    else{ //if error 101 or 202 is detected
      //dont write error 303 to eeprom
    }

  }
  Serial.println("read state done");
  delay(500);
  lcd.clear();

}


void loop() {
  if(danger_flag == 1){
    danger();
    
  }
  
  else{
    prev_adc = adc;
    danger_flag = 0;
    

    Date_time();
    adc = analogRead(A0);
    delay(10);

    if (adc != prev_adc){
      prev_adc = adc;
      send_duty(); 
      error(202);
    }

  }

  
}