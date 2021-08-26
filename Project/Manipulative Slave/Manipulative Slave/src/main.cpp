#include <Arduino.h>
#include <LiquidCrystal.h>

const int rs = 9, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//int adc;
int flag = 0;
int duty;

void timer_initial(){
  cli();
  // TCCR1A = 0;
  // TCCR1B = 0;
  TCNT2 = 0;
  OCR2A = 10;

  
  TCCR2A=(1<<WGM20)|(1<<WGM21)|(1<<COM2A0)|(1<<COM2A1); //set on comapre match
  TCCR2B = (2<<CS20)|(0<<WGM22); //fast pwm   prescale = 8
  TIMSK2|=(1<<OCIE2A);
    
  sei();
}

ISR(TIMER2_COMPA_vect) {
  OCR2A = 255 - duty;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  pinMode(11, OUTPUT); //pwm signal output
  Serial.begin(9600);
  
  timer_initial();
  lcd.begin(16, 2);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(Serial.available()){
    duty = Serial.read();
  }
  
  
}