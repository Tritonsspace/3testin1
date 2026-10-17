// ------ Switch Pin ------
#define sw 0

// ------ Function Wait_Start ------
void waitForStart(unsigned int hold = 200) {
  unsigned long timeSince = millis();
  while (millis() - timeSince < hold) 
  {
    timeSince = digitalRead(0) ? millis() : timeSince;
  }
  delay(500);
}
// ------ Function Sw_Start ------
int sw_Start()
{
  return digitalRead(0);
}
