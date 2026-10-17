#include <Arduino.h>
#include <EasyKids3in1.h>
#define Green D0
#define Yellow D1
#define Red D2
void setup() {
  EasyKids_Setup();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(Green, OUTPUT);
  pinMode(Yellow, OUTPUT);
  pinMode(Red, OUTPUT);
  waitForStart();
  beep();
}


void loop() {
  digitalWrite(Green, HIGH);  
  digitalWrite(Yellow, HIGH);
  digitalWrite(Red, HIGH);  
  delay(100);                     
  digitalWrite(Yellow, LOW);   
  digitalWrite(Green, LOW);  
  digitalWrite(Red, LOW);   
  delay(100);             
}