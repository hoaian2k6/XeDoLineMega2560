#include <Arduino.h>
#include <QTRSensors.h>

QTRSensors qtr;

const uint8_t pwmRight = 10; // PB4
const uint8_t dirRight = 21; // PD0
const uint8_t pwmLeft = 13; // PB7
const uint8_t dirLeft = 20; // PD1

const uint8_t servoPin = 5;  // PE3
const uint8_t led1 = 41; // PG3
const uint8_t led0 = 40; // PG4
const uint8_t button = 4;  // PE2

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
const uint8_t SensorPins[SensorCount] = {A0, A1, A2, A3, A4, A5, A6, A7};

void setup()
{
  Serial.begin(9600);
  qtr.setTypeAnalog();
  qtr.setSensorPins(SensorPins, SensorCount);
}

void loop()
{
  qtr.read(sensorValues);

  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }

  Serial.println();
  delay(250);
}