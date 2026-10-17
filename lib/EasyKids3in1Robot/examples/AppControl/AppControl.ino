#include <EasyKids3in1.h>

void setup() {
  EasyKids_Setup();
  AppControl_Begin(); // App V7RC Turbo
}
void loop() {
  appControl(); //Mode Game ADV
  //appReadValue(); // Read Value JoyStick & Button
  if (ch1 > 1750) {
    int speed = map(ch1, 1750, 2000, 0, 100);
    turnRight(speed);
  } else if (ch1 < 1250) {
    int speed = map(ch1, 1250, 1000, 0, 100);
    turnLeft(speed);
  } else if (ch2 < 1450) {
    int speed = map(ch2, 1450, 1000, 0, 100);
    backward(speed);
  } else if (ch2 > 1550) {
    int speed = map(ch2, 1550, 2000, 0, 100);
    forward(speed);
  } else if (ch3 == 1500 && ch4 == 1000) {
    slideLeft(50);
  } else if (ch3 == 1500 && ch4 == 2000) {
    slideRight(50);
  } else if (ch3 == 2000 && ch4 == 1500) {
    // Statement
  } else if (ch3 == 1000 && ch4 == 1500) {
    // Statement
  } else if (ch3 == 2000 && ch4 == 1000) {
    // Statement
  } else if (ch3 == 2000 && ch4 == 2000) {
    // Statement
  } else if (ch3 == 1000 && ch4 == 1000) {
    // Statement
  } else if (ch3 == 1000 && ch4 == 2000) {
    // Statement
  } else {
    motorStopAll();
  }
}
void appReadValue(){
   Serial.print("ch1 = "); 
   Serial.print(ch1); 
   Serial.print(" "); 
   Serial.print("ch2 = "); 
   Serial.print(ch2); 
   Serial.print(" "); 
   Serial.print("ch3 = "); 
   Serial.print(ch3); 
   Serial.print(" "); 
   Serial.print("ch4 = "); 
   Serial.print(ch4); 
   Serial.println(" "); 

}