#include <EasyKids3in1.h>

void setup() 
{
  EasyKids_Setup();
  beginSerialRX();
}

void loop() 
{

  /* Read string */
  Serial.println(readDataString());
  delay(100);

  /* Read number */
  // Serial.println(readDataFloat());
  // delay(100);
}
