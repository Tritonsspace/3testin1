#include <EasyKids3in1.h>

/* 
<<<<< Command >>>>>
  tone(BUZZER, frequency, duration);
	noTone(BUZZER);
  beep();
  welcomeSong(); 

  pacmanSong();
  happyBirthDaySong();
  christmasSong();
  marioSong();

*/

void setup() {
  EasyKids_Setup();
  display3in1();

}

void loop() {

  waitForStart();
  tone(BUZZER, 500, 100);
	noTone(BUZZER);
  beep();
  welcomeSong(); 
  pacmanSong();
  happyBirthDaySong();
  christmasSong();
  marioSong();

}
