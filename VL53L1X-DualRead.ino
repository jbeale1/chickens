/*
Read two VL53L1X distance sensors on same I2C bus
Note: Address change is stored in RAM, and resets with either init() or power cycle.
Based on https://forum.pololu.com/t/using-multiple-vl53l1x-with-arduino/15825/5
*/

#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor1;
VL53L1X sensor2;

#define ADR1 0x29  // default address VL53L1X always starts with on powerup
#define ADR2 0x30

#define XSHUT1 3
#define XSHUT2 5

void setup()
{
  pinMode(XSHUT1, OUTPUT);
  pinMode(XSHUT2, OUTPUT);
  digitalWrite(XSHUT1, LOW); // HIGH = enabled
  digitalWrite(XSHUT2, LOW);

  while (!Serial) {}
  Serial.begin(115200);
  delay(500);
  Wire.begin();
  Wire.beginTransmission(ADR1);
  // Wire.setClock(400000); // use 400 kHz I2C (what is default?)

  digitalWrite(XSHUT2, HIGH); // HIGH = enabled
  delay(150);
  if (!sensor2.init())
  {
    Serial.println("Failed to detect and initialize sensor2 !");
    while (1);
  }

  delay(100);
  sensor2.setAddress(ADR2);
  delay(50);

  digitalWrite(XSHUT1, HIGH); // HIGH = enabled
  delay(150);
  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor1 !");
    while (1);
  }
  delay(100);

  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor1.setDistanceMode(VL53L1X::Long);
  sensor1.setMeasurementTimingBudget(50000);

  sensor2.setDistanceMode(VL53L1X::Long);
  sensor2.setMeasurementTimingBudget(50000);

  sensor1.setTimeout(500);
  sensor2.setTimeout(500);

  // Start continuous readings at a rate of one measurement every X ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  // sensor1.startContinuous(50);
  sensor1.startContinuous(250);
  sensor2.startContinuous(250);
}

void loop()
{
  Serial.print(sensor1.read());
  if (sensor1.timeoutOccurred()) { Serial.print(" --- "); }

  Serial.print(", ");
  Serial.print(sensor2.read());
  if (sensor1.timeoutOccurred()) { Serial.print(" --- "); }

  Serial.println();

  // delay(500);
}
