
#include <Arduino.h>
#include <EasyKids3in1.h>
#include <Wire.h>
#include <MFRC522_I2C.h>
#include <Preferences.h>

#define Green D0
#define Yellow D1
#define Red D2
#define RST_PIN 14

Preferences preferences;
MFRC522_I2C mfrc522(0x28, RST_PIN);

unsigned long previousHoldTime = 0;
unsigned long previousLightTime = 0;

int mode = 0;

String uuid = "";
String correctUuid = "21592251154";

int masterCode[3] = {20, 40, 60};
int enteredCode[3] = {0, 0, 0};
int codeIndex = 0;
int number = 0;

bool holding = false;
bool longPressed = false;

void drawScreen_1(void) {
    display.fillScreen(0x0);
    display.setTextColor(0xFFFF);
    display.setTextSize(2);
    display.setTextWrap(false);
    display.setCursor(20, 7);
    display.print("");

    display.setTextSize(9);
    display.setCursor(15, 56);
    display.print("00");

    display.setTextSize(2);
    display.setCursor(19, 22);
    display.print("__ __ __");
}

void drawPin() {
    display.fillScreen(0x0);
    display.setTextColor(0xFFFF);
    display.setTextSize(2);
    display.setCursor(19, 22);

    for (int i = 0; i < 3; i++) {
        if (i < codeIndex) {
            if (enteredCode[i] < 10) display.print("0");
            display.print(enteredCode[i]);
        }
        else if (i == codeIndex) {
            if (number < 10) display.print("0");
            display.print(number);
        }
        else {
            display.print("__");
        }

        if (i < 2) display.print(" ");
    }

    display.setTextSize(9);
    display.setCursor(15, 56);
    if (number < 10) display.print("0");
    display.print(number);
}

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
        Serial.println(F("WARNING: Communication failure"));
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
    pinMode(sw, INPUT_PULLUP);
    pinMode(D3, INPUT);

    preferences.begin("rfid", false);
    correctUuid = preferences.getString("uuid", correctUuid);

    waitForStart();

    digitalWrite(Green, 0);
    digitalWrite(Yellow, 0);
    digitalWrite(Red, 0);

    drawScreen_1();
    beep();
}

void loop() {
    unsigned long currentHoldTime = millis();

    if (digitalRead(sw) == LOW) {
        if (!holding) {
            holding = true;
            longPressed = false;
            previousHoldTime = currentHoldTime;
        }

        if (currentHoldTime - previousHoldTime >= 3000 &&
            !longPressed) {

            longPressed = true;

            if (mode == 0 || mode == 3) {
                mode = 1;
                codeIndex = 0;
                number = 0;
                digitalWrite(Green, 0);
                digitalWrite(Yellow, 0);
                beep();
                drawPin();
            }
            else {
                mode = 0;
                digitalWrite(Yellow, 0);
                drawScreen_1();
                beep();
            }
        }
    }
    else {
        if (holding && !longPressed && mode == 1 &&
            currentHoldTime - previousHoldTime >= 40) {

            enteredCode[codeIndex] = number;
            codeIndex++;
            beep();

            if (codeIndex == 3) {
                if (enteredCode[0] == masterCode[0] &&
                    enteredCode[1] == masterCode[1] &&
                    enteredCode[2] == masterCode[2]) {

                    mode = 2;
                    digitalWrite(Yellow, 1);

                    display.fillScreen(0x0);
                    display.setTextSize(2);
                    display.setCursor(10, 40);
                    display.print("MASTER MODE");
                    display.setCursor(10, 70);
                    display.print("SCAN CARD");
                    beep();
                }
                else {
                    mode = 0;
                    digitalWrite(Red, 1);
                    beep();
                    delay(100);
                    beep();
                    delay(900);
                    digitalWrite(Red, 0);
                    drawScreen_1();
                }
            }
            else {
                drawPin();
            }
        }

        holding = false;
    }

    // VR selection
    if (mode == 1) {
        int newNumber = map(analogRead(VR), 0, 4095, 0, 99);
        newNumber = constrain(newNumber, 0, 99);

        if (newNumber != number) {
            number = newNumber;
            drawPin();
        }
    }

    // Photoresistor mode
    if (mode == 3 &&
        currentHoldTime - previousLightTime >= 200) {

        previousLightTime = currentHoldTime;
        int lightValue = analogRead(D3);

        display.fillScreen(0x0);
        display.setTextColor(0xFFFF);
        display.setTextSize(2);
        display.setCursor(10, 20);
        display.print("LIGHT");

        display.setTextSize(4);
        display.setCursor(10, 60);
        display.print(lightValue);

        Serial.print("Light: ");
        Serial.println(lightValue);
    }

    if (mode == 1) return;

    // Original RFID reading
    if (!mfrc522.PICC_IsNewCardPresent() ||
        !mfrc522.PICC_ReadCardSerial()) {
        delay(50);
        return;
    }

    uuid = "";
    Serial.print(F("Card UID:"));

    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uuid += mfrc522.uid.uidByte[i];
    }

    Serial.println(uuid);

    if (mode == 2) {
        // Master mode: register any scanned card
        correctUuid = uuid;
        preferences.putString("uuid", correctUuid);

        digitalWrite(Green, 1);
        beep();
        delay(1000);
        digitalWrite(Green, 0);

        display.fillScreen(0x0);
        display.setTextSize(2);
        display.setCursor(10, 40);
        display.print("CARD SAVED");
    }
    else if (uuid == correctUuid) {
        digitalWrite(Green, 1);
        beep();
        delay(1000);
        digitalWrite(Green, 0);

        if (mode == 3) {
            mode = 0;
            drawScreen_1();
        }
        else {
            mode = 3;
        }
    }
    else {
        digitalWrite(Red, 1);
        beep();
        delay(10);
        beep();
        delay(990);
        digitalWrite(Red, 0);
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    Serial.println();
}
