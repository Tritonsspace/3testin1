int valSensor1 = 0;
int valSensor2 = 0;
int valSensor3 = 0;
int valSensor4 = 0;
int valSensor5 = 0;
int valSensor6 = 0;

int rightMotor;
int leftMotor;
int NumSensor = 6;
bool invertedLine = false;
int Pin_Setup[7] = {1, 2, 3, 4, 5, 6};
int Sensor_Min[] = {10, 10, 10, 10, 10, 10};
int Sensor_Max[] = {4000, 4000, 4000, 4000, 4000, 4000};

float KP;
float KD;
uint16_t setPoint = (NumSensor - 1) * 100 / 2;
uint32_t lastPosition = (NumSensor - 1) * 100 / 2;
float errors = 0;
float output = 0;
float derivative;
float previous_error = setPoint;

void setSensorMin( int s1, int s2 = 10, int s3 = 10, int s4 = 10, int s5 = 10, int s6 = 10)
{
    Sensor_Min[0] = s1;
    Sensor_Min[1] = s2;
    Sensor_Min[2] = s3;
    Sensor_Min[3] = s4;
    Sensor_Min[4] = s5;
    Sensor_Min[5] = s6;

}

void setSensorMax( int s1, int s2 = 4000, int s3 = 4000, int s4 = 4000, int s5 = 4000, int s6 = 4000)
{
    Sensor_Max[0] = s1;
    Sensor_Max[1] = s2;
    Sensor_Max[2] = s3;
    Sensor_Max[3] = s4;
    Sensor_Max[4] = s5;
    Sensor_Max[5] = s6;
}

void sensorNum(int num)
{
    NumSensor = num;
    setPoint = (NumSensor - 1) * 100 / 2;
    lastPosition = (NumSensor - 1) * 100 / 2;
    pwmMotor.setPWMFreq(100);
}

void whiteLine() { invertedLine = true; }
void blackLine() { invertedLine = false; }

int readline()
{
    int onLine = 0; // Check if robot still on line (Atleast 1 sensor)
    long avg = 0;
    long sum = 0;
    for (int i = 0; i < NumSensor; i++)
    {
        long value = 0;

        value = clamp(map(analog(Pin_Setup[i]), Sensor_Min[i], Sensor_Max[i], 1, 100), 1, 100);
        if (invertedLine) // White line
        { 
            value = 101 - value;
        }

        if(value < 50)
        {                                                                  // More than 800 = White
            onLine = 1;
            avg += value * (i * 100);
            sum += value;
        }

        // Serial.print("Sensor ");
        // Serial.print(i);
        // Serial.print(": ");
        // Serial.print(value);
        // Serial.print(" / ");
    }
    // Serial.println("");

    if (onLine == 0)
    {
        if (((lastPosition >= ((NumSensor - 1) * 100) / 2) && invertedLine) || // White line
            ((lastPosition <= ((NumSensor - 1) * 100) / 2) && !invertedLine))  // Black line
        {
            lastPosition = 0;
        }
        else
        {
            lastPosition = (NumSensor - 1) * 100; // Middle value
        }
    }
    else
    {
        lastPosition = (avg / sum);
    }
    return round(((NumSensor - 1) * 100) - lastPosition);
}

int readline2()
{
    int onLine = 0; // Check if robot still on line (Atleast 1 sensor)
    long avg = 0;
    long sum = 0;
    for (int i = 0; i < NumSensor; i++)
    {
        long value = 0;

        value = clamp(map(analog(Pin_Setup[i]), Sensor_Min[i], Sensor_Max[i], 1, 100), 1, 100);
        if (invertedLine) // White line
        { 
            value = 101 - value;
        }

        if(value < 50)
        {                                                                  // More than 800 = White
            onLine = 1;
            avg += value * (i * 100);
            sum += value;
        }

        // Serial.print("Sensor ");
        // Serial.print(i);
        // Serial.print(": ");
        // Serial.print(value);
        // Serial.print(" / ");
    }
    // Serial.println("");

    if (onLine == 0)
    {
       lastPosition = ((NumSensor - 1) * 100) / 2;
    }
    else
    {
        lastPosition = (avg / sum);
    }
    return round(((NumSensor - 1) * 100) - lastPosition);
}

void trackLine(int min_speed, float iKP, float iKD)
{
    KP = iKP / 10;
    KD = iKD / 10;

    errors = (readline() - setPoint);
    Serial.println(readline());
    derivative = (errors - previous_error);
    output = (KP * errors) + (KD * derivative);
    previous_error = errors;
    leftMotor = clamp(min_speed - output, -100, 100);
    rightMotor = clamp(min_speed + output, -100, 100);

    motor(1, leftMotor);
    motor(2, leftMotor);
    motor(3, rightMotor);
    motor(4, rightMotor);
}

void trackDashedLine(int min_speed, float iKP, float iKD)
{
    KP = iKP / 10;
    KD = iKD / 10;

    errors = (readline2() - setPoint);
    //Serial.println(readline2());
    derivative = (errors - previous_error);
    output = (KP * errors) + (KD * derivative);
    previous_error = errors;
    leftMotor = clamp(min_speed - output, -100, 100);
    rightMotor = clamp(min_speed + output, -100, 100);

    motor(1, leftMotor);
    motor(2, leftMotor);
    motor(3, rightMotor);
    motor(4, rightMotor);
}

void lineTime(int speed, float iKP, float iKD, long setTime)
{

    long timeSince = 0;
    timeSince = millis();
    while ((millis() - timeSince) < setTime)
    {
        trackLine(speed, iKP, iKD);
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void dashedLineTime(int speed, float iKP, float iKD, long setTime)
{

    long timeSince = 0;
    timeSince = millis();
    while ((millis() - timeSince) < setTime)
    {
        trackDashedLine(speed, iKP, iKD);
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void lineCross(int setSpeed, float iKP, float iKD)
{
    int righterVal = analog(Pin_Setup[0]);
    int rightVal = analog(Pin_Setup[1]);

    int lefterVal = analog(Pin_Setup[NumSensor - 1]);
    int leftVal = analog(Pin_Setup[NumSensor - 2]);

    int avgRighterVal = (Sensor_Max[0] + Sensor_Min[0]) / 2;
    int avgRightVal = (Sensor_Max[1] + Sensor_Min[1]) / 2;

    int avgLefterVal = (Sensor_Max[NumSensor - 1] + Sensor_Min[NumSensor - 1]) / 2;
    int avgLeftVal = (Sensor_Max[NumSensor - 2] + Sensor_Min[NumSensor - 2]) / 2;

    if (invertedLine)
    {
        while (righterVal < avgRighterVal || rightVal < avgRightVal || lefterVal < avgLefterVal || leftVal < avgLeftVal)
        {
            righterVal = analog(Pin_Setup[0]);
            rightVal = analog(Pin_Setup[1]);
            lefterVal = analog(Pin_Setup[NumSensor - 1]);
            leftVal = analog(Pin_Setup[NumSensor - 2]);
            trackLine(setSpeed, iKP, iKD);
        }
    }
    else
    {
        while (righterVal > avgRighterVal || rightVal > avgRightVal || lefterVal > avgLefterVal || leftVal > avgLeftVal)
        {
            righterVal = analog(Pin_Setup[0]);
            rightVal = analog(Pin_Setup[1]);
            lefterVal = analog(Pin_Setup[NumSensor - 1]);
            leftVal = analog(Pin_Setup[NumSensor - 2]);
            trackLine(setSpeed, iKP, iKD);
        }
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void dashedLineCross(int setSpeed, float iKP, float iKD)
{
    int righterVal = analog(Pin_Setup[0]);
    int rightVal = analog(Pin_Setup[1]);

    int lefterVal = analog(Pin_Setup[NumSensor - 1]);
    int leftVal = analog(Pin_Setup[NumSensor - 2]);

    int avgRighterVal = (Sensor_Max[0] + Sensor_Min[0]) / 2;
    int avgRightVal = (Sensor_Max[1] + Sensor_Min[1]) / 2;

    int avgLefterVal = (Sensor_Max[NumSensor - 1] + Sensor_Min[NumSensor - 1]) / 2;
    int avgLeftVal = (Sensor_Max[NumSensor - 2] + Sensor_Min[NumSensor - 2]) / 2;

    if (invertedLine)
    {
        while (righterVal < avgRighterVal || rightVal < avgRightVal || lefterVal < avgLefterVal || leftVal < avgLeftVal)
        {
            righterVal = analog(Pin_Setup[0]);
            rightVal = analog(Pin_Setup[1]);
            lefterVal = analog(Pin_Setup[NumSensor - 1]);
            leftVal = analog(Pin_Setup[NumSensor - 2]);
            trackDashedLine(setSpeed, iKP, iKD);
        }
    }
    else
    {
        while (righterVal > avgRighterVal || rightVal > avgRightVal || lefterVal > avgLefterVal || leftVal > avgLeftVal)
        {
            righterVal = analog(Pin_Setup[0]);
            rightVal = analog(Pin_Setup[1]);
            lefterVal = analog(Pin_Setup[NumSensor - 1]);
            leftVal = analog(Pin_Setup[NumSensor - 2]);
            trackDashedLine(setSpeed, iKP, iKD);
        }
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void lineFork(int setSpeed, float iKP, float iKD)
{
    int outerRightVal = analog(Pin_Setup[0]);
    int outerLeftVal = analog(Pin_Setup[NumSensor - 1]);

    int avgOuterRightVal = (Sensor_Max[0] + Sensor_Min[0]) / 2;
    int avgOuterLeftVal = (Sensor_Max[NumSensor - 1] + Sensor_Min[NumSensor - 1]) / 2;

    if (invertedLine)
    {
        while (outerRightVal < avgOuterRightVal || outerLeftVal < avgOuterLeftVal)
        {
            outerRightVal = analog(Pin_Setup[0]);
            outerLeftVal = analog(Pin_Setup[NumSensor - 1]);

            trackLine(setSpeed, iKP, iKD);
        }
    }
    else
    {
        while (outerRightVal > avgOuterRightVal || outerLeftVal > avgOuterLeftVal)
        {
            outerRightVal = analog(Pin_Setup[0]);
            outerLeftVal = analog(Pin_Setup[NumSensor - 1]);

            trackLine(setSpeed, iKP, iKD);
        }
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void line90Left(int setSpeed, float iKP, float iKD)
{
    int leftVal = analog(Pin_Setup[NumSensor - 2]);
    int leftestVal = analog(Pin_Setup[NumSensor - 1]);

    int avgLeftVal = (Sensor_Max[NumSensor - 2] + Sensor_Min[NumSensor - 2]) / 2;
    int avgLeftestVal = (Sensor_Max[NumSensor - 1] + Sensor_Min[NumSensor - 1]) / 2;

    if (invertedLine)
    {
        while (leftVal < avgLeftVal || leftestVal < avgLeftestVal)
        {
            leftVal = analog(Pin_Setup[NumSensor - 2]);
            leftestVal = analog(Pin_Setup[NumSensor - 1]);

            trackLine(setSpeed, iKP, iKD);
        }
    }
    else
    {
        while (leftVal > avgLeftVal || leftestVal > avgLeftestVal)
        {
            leftVal = analog(Pin_Setup[NumSensor - 2]);
            leftestVal = analog(Pin_Setup[NumSensor - 1]);

            trackLine(setSpeed, iKP, iKD);
        }
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void line90Right(int setSpeed, float iKP, float iKD)
{
    int rightVal = analog(Pin_Setup[0]);
    int rightestVal = analog(Pin_Setup[1]);

    int avgRightVal = (Sensor_Max[0] + Sensor_Min[0]) / 2;
    int avgRightestVal = (Sensor_Max[1] + Sensor_Min[1]) / 2;

    if (invertedLine)
    {
        while (rightVal < avgRightVal || rightestVal < avgRightestVal)
        {
            rightVal = analog(Pin_Setup[0]);
            rightestVal = analog(Pin_Setup[1]);
            trackLine(setSpeed, iKP, iKD);
        }
    }
    else
    {
        while (rightVal > avgRightVal || rightestVal > avgRightestVal)
        {
            rightVal = analog(Pin_Setup[0]);
            rightestVal = analog(Pin_Setup[1]);
            trackLine(setSpeed, iKP, iKD);
        }
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void lineTurnLeft(int speedM)
{
    spinLeft(speedM);
    delay(70);
    if (invertedLine)
    {
        do
        {
            spinLeft(speedM);
        } while (analog(Pin_Setup[0]) < (Sensor_Max[0] + Sensor_Min[0]) / 2);
    }
    else
    {
        do
        {
            spinLeft(speedM);
        } while (analog(Pin_Setup[0]) > (Sensor_Max[0] + Sensor_Min[0]) / 2);
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void lineTurnRight(int speedM)
{
    spinRight(speedM);
    delay(70);
    if (invertedLine)
    {
        do
        {
            spinRight(speedM);
        } while (analog(Pin_Setup[0]) < (Sensor_Max[0] + Sensor_Min[0]) / 2);
    }
    else
    {
        do
        {
            spinRight(speedM);
        } while (analog(Pin_Setup[0]) > (Sensor_Max[0] + Sensor_Min[0]) / 2);
    }
    // motor(1, 0);
    // motor(4, 0);
    motorStopAll();
}

void readSensor()
{
    displayClear();
    while (1)
    {
        display.setTextFont(GLCD);
        display.setTextSize(2);
        display.setCursor(15, 5);
        display.setTextColor(0xF81F);
        display.print("Read Sensor 0 - 6");

        // ------- Sensor 1 -------
        display.setTextFont(GLCD);
        display.setTextSize(2);
        display.setCursor(20, 60);
        display.setTextColor(0x0);
        display.print("Sensor(1) = ");
        display.print(valSensor1);
        valSensor1 = analog(Pin_Setup[0]);
        display.setTextSize(2);
        display.setCursor(20, 60);
        display.setTextColor(0x07FF);
        display.print("Sensor(1) = ");
        display.print(valSensor1);

        // ------- Sensor 2 -------
        display.setTextFont(GLCD);
        display.setTextSize(2);
        display.setCursor(20, 90);
        display.setTextColor(0x0);
        display.print("Sensor(2) = ");
        display.print(valSensor2);
        valSensor2 = analog(Pin_Setup[1]);
        display.setTextSize(2);
        display.setCursor(20, 90);
        display.setTextColor(0x07FF);
        display.print("Sensor(2) = ");
        display.print(valSensor2);

        // ------- Sensor 3 -------
        display.setTextFont(GLCD);
        display.setTextSize(2);
        display.setCursor(20, 120);
        display.setTextColor(0x0);
        display.print("Sensor(3) = ");
        display.print(valSensor3);
        valSensor3 = analog(Pin_Setup[2]);
        display.setTextSize(2);
        display.setCursor(20, 120);
        display.setTextColor(0x07FF);
        display.print("Sensor(3) = ");
        display.print(valSensor3);

        // Serial
        Serial.print("| Sensor 1 = ");
        Serial.print(valSensor1);
        Serial.print("| Sensor 2 = ");
        Serial.print(valSensor2);
        Serial.print("| Sensor 3 = ");
        Serial.print(valSensor3);

        // ------- Sensor 4 -------
        display.setTextFont(GLCD);
        display.setTextSize(2);
        display.setCursor(20, 150);
        display.setTextColor(0x0);
        display.print("Sensor(4) = ");
        display.print(valSensor4);
        valSensor4 = analog(Pin_Setup[3]);
        display.setTextSize(2);
        display.setCursor(20, 150);
        display.setTextColor(0x07FF);
        display.print("Sensor(4) = ");
        display.print(valSensor4);

        // Serial
        Serial.print("| Sensor 4 = ");
        Serial.print(valSensor4);


        // ------- Sensor 5 -------
        display.setTextFont(GLCD);
        display.setTextSize(2);
        display.setCursor(20, 180);
        display.setTextColor(0x0);
        display.print("Sensor(5) = ");
        display.print(valSensor5);
        valSensor5 = analog(Pin_Setup[4]);
        display.setTextSize(2);
        display.setCursor(20, 180);
        display.setTextColor(0x07FF);
        display.print("Sensor(5) = ");
        display.print(valSensor5);

        // Serial
        Serial.print("| Sensor 5 = ");
        Serial.print(valSensor5);

        // ------- Sensor 6 -------
        display.setTextFont(GLCD);
        display.setTextSize(2);
        display.setCursor(20, 210);
        display.setTextColor(0x0);
        display.print("Sensor(6) = ");
        display.print(valSensor6);
        valSensor6 = analog(Pin_Setup[5]);
        display.setTextSize(2);
        display.setCursor(20, 210);
        display.setTextColor(0x07FF);
        display.print("Sensor(6) = ");
        display.print(valSensor6);

        // Serial
        Serial.print("| Sensor 6 = ");
        Serial.print(valSensor6);

        Serial.println(" ");
        delay(50);
    }
}