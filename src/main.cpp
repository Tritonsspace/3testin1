#include <Arduino.h>
#include <EasyKids3in1.h>
#include <Wire.h>
#include <MFRC522_I2C.h>
#define Green D0
#define Yellow D1
#define Red D2
#define RST_PIN 14
String uuid = "";
MFRC522_I2C mfrc522(0x28, RST_PIN); 
void ShowReaderDetails() {
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}
void setup() {
  EasyKids_Setup();
  Wire.begin();                
  mfrc522.PCD_Init(); 
  ShowReaderDetails();   
  pinMode(Green, OUTPUT);
  pinMode(Yellow, OUTPUT);
  pinMode(Red, OUTPUT);
  waitForStart();
  beep();
}
void loop() {
if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }          
}
MFRC522_I2C mfrc522(0x28, RST_PIN);   
void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  uuid = "";
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    uuid += mfrc522.uid.uidByte[i];
  } 
  Serial.println();
  }

