#include <Arduino.h>
#include <CAN.h>
#include <PS4Controller.h>

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  PS4.begin("E4:65:B8:7E:05:4A");

  CAN.setPins(4, 5); // 16,17ピンはつかえない(4,5ピンは使えた)
  if (!CAN.begin(1000E3))
  {
    Serial.println("Starting CAN failed!");
    while (1)
      ;
  }

  volatile uint32_t *pREG_IER = (volatile uint32_t *)0x3ff6b010;
  *pREG_IER &= ~(uint8_t)0x10;

  Serial.println("Ready");
}

void loop()
{
  int lx;
  int ly;

  lx = PS4.LStickX();
  ly = PS4.LStickY();

  CAN.beginPacket(0x01);
  CAN.write(lx);
  CAN.write(ly);
  CAN.write(0);
  CAN.write(0);
  CAN.write(0);
  CAN.write(0);
  CAN.write(0);
  CAN.write(0);
  CAN.endPacket();

  lx = 0;
  ly = 0;
}