#include <EasyKids3in1.h>

/* 
Connect Analog 6 Sensor >>> A1 A2 A3 A4 A5 A6 

<<<<< Command >>>>>
  sensorNum(num); 
  whiteLine();
  blackLine();
  readSensor();
  setSensorMin(s1, s2, s3, s4, s5, s6);
  setSensorMax(s1, s2, s3, s4, s5, s6);

  trackLine(Speed, KP, KD);  
  lineTime (Speed, KP, KD, Time);
  lineCross(Speed, KP, KD);
  line90Left(Speed, KP, KD);
  line90Right(Speed, KP, KD);
  lineTurnLeft(Speed);
  lineTurnRight(Speed);

  trackDashedLine(Speed, KP, KD);
  dashedLineTime (Speed, KP, KD, Time);
  dashedLineCross(Speed, KP, KD);
*/

void setup() {
  EasyKids_Setup();
  // display3in1();
  // blackLine();
  // //whiteLine();
  // setSensorMin(975, 900, 385, 1229, 912, 723);        // Min Line Value >>> A1 A2 A3 A4 A5 A6
  // setSensorMax(4023, 3875, 2380, 4065, 4023, 3460);  // Max Line Value >>> A1 A2 A3 A4 A5 A6
}

void loop() {

  // readSensor();  //Show Value Sensor via LCD Display
  waitForStart();
  beep();
  
  // lineTime(20, 1.0, 1.0, 1000);
  // delay(500);

  // lineTurnLeft(50);
  // delay(2000);

  // lineCross(30, 1.0, 1.0);
}
