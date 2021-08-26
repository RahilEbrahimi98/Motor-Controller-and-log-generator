#include <Arduino.h>

int val = 0; 
int new_val = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT); //interrupt
  digitalWrite(7, LOW);
  Serial.begin(9600);
  digitalWrite(7, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(A0);  //ranges between 0 to 308  for temp 0 to 150
  new_val = val * 4.88;
  new_val = new_val / 10;
  //new_val = map(val, 0, 308, 0, 150);
  Serial.println(new_val);
  if (new_val <= 30){
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(9, LOW);
    digitalWrite(8, HIGH);
    digitalWrite(7, LOW);
  }
  else if (new_val <= 60){
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(9, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(7, LOW);
  }
  else if (new_val <= 90){
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(7, LOW);
  }
  else if (new_val <= 120){
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(7, LOW);
  }
  else if (new_val <= 150){
    digitalWrite(12, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(7, HIGH); //enable external interrupt
    // 6th LED blink 
    digitalWrite(13, HIGH); 
    delay(30);
    digitalWrite(13, LOW);
    delay(30);
    
  }


}