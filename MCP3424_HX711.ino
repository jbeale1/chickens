# Environmental montor for chicken house: 
# 1 load cell on HX711 24-bit ADC (bit-bang 1-way SPI)
# 2 LM35 temp sensors on MCP3424 18-bit ADC (I2C port)
# J.Beale 2023-July-30

#include <Wire.h>
#include <MCP342x.h>
#include "HX711.h"

HX711 scale1;
uint8_t dat1 = 2; // pins going to HX711 #1
uint8_t clk1 = 3;
float scalefac1 = - 1.0/475.4; // grams per ADC count on 5kg load cell
float tareA = 0;

// 0x68 is the default address for all MCP342x devices
//uint8_t address = 0x69;
uint8_t address = 0x68;
MCP342x adc = MCP342x(address);

void setup(void)
{
  Serial.begin(9600);
  Wire.begin();

  // Enable power for MCP342x (needed for FL100 shield only)
  //pinMode(9, OUTPUT);
  //digitalWrite(9, HIGH);
  
  // Reset devices
  MCP342x::generalCallReset();
  delay(1); // MC342x needs 300us to settle, wait 1ms
  
  // Check device present
  Wire.requestFrom(address, (uint8_t)1);
  if (!Wire.available()) {
    Serial.print("No device found at address ");
    Serial.println(address, HEX);
    while (1);
  }

  scale1.begin(dat1, clk1);
  float sum=0;
  int loops = 10;
  for (int i = 0; i < loops; i++)
  {
    sum += scale1.read_medavg(7);
  }
  tareA = sum / loops;
  Serial.print("Tare1A = ");
  Serial.println(tareA);
}

float getTemp(const MCP342x::Channel ch, int averages) {
  // 4.096 V = 2^18 counts, so 64000 counts per volt
  // 18 bits, gain=1: 63943 counts per volt
  float vscale = 1.0/(4*64000); // volts per ADC count @ 18bits, G=4
  long value = 0;
  MCP342x::Config status;

  uint8_t err;
  float degC = 0;
  for (int i=0;i<averages;i++) {
      // Initiate a conversion; convertAndRead() will wait until it can be read
      do {
          err = adc.convertAndRead(ch, MCP342x::oneShot,
                  MCP342x::resolution18, MCP342x::gain4,
                  1000000, value, status);
          if (err) {
            Serial.print("Convert error: ");
            Serial.println(err);
          }
      } while (err != 0);
      degC += value * vscale * 100;
  }
  degC /= averages;
  return degC;
}

void loop(void)
{
  float f, grams1;
  f = scale1.read_average(25); // average together this many readings
  grams1 = (f - tareA) * scalefac1;
  Serial.print(grams1);
  Serial.print(", ");

  float degC1 = getTemp(MCP342x::channel1, 5);
  float degC2 = getTemp(MCP342x::channel2, 5);

  Serial.print(degC1,5);
  Serial.print(", ");
  Serial.print(degC2,5);
  Serial.println();
}

