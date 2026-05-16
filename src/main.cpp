#include <Arduino.h>
#include <CAN.h>
#include <PS4Controller.h>

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  PS4.begin("E4:65:B8:7E:05:4A");

  CAN.setPins(4, 5);      // 16,17ピンはつかえない(4,5ピンは使えた)
  if (!CAN.begin(1000E3)) // 1000kbpsで開始
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
  int16_t motor[4] = {0};

  int lx;
  int ly;
  int x_up_straight;
  int x_down_straight;
  int ry_straight;
  int ly_straight;
  int l2;
  int r2;
  int crossState;

  if (PS4.isConnected())
  {
    lx = PS4.LStickX();
    ly = PS4.LStickY();
    x_up_straight = PS4.Up();
    x_down_straight = PS4.Down();
    ry_straight = PS4.Right();
    ly_straight = PS4.Left();
    l2 = PS4.L2Value();
    r2 = PS4.R2Value();
    crossState = PS4.Cross();

    if (abs(lx) < 10)
      lx = 0;
    if (abs(ly) < 10)
      ly = 0;
    if (abs(l2) < 10)
      l2 = 0;
    if (abs(r2) < 10)
      r2 = 0;

    if (crossState == HIGH)
    {
      memset(motor, 0, sizeof(motor));
    }
    else
    {
      constexpr float INV_SQRT2 = 0.70710678f;
      int rot = (l2 - r2);
      float gain = 20.0f;

      float v1 = ((-lx + ly) * INV_SQRT2 + rot) * gain;
      float v2 = ((lx + ly) * INV_SQRT2 + rot) * gain;
      float v3 = ((lx - ly) * INV_SQRT2 + rot) * gain;
      float v4 = ((-lx - ly) * INV_SQRT2 + rot) * gain;

      float v[4] = {v1, v2, v3, v4};

      for (int i = 0; i < 4; i++)
      {
        motor[i] = (int16_t)constrain(v[i], -2999, 2999);
      }
    }
  }

  CAN.beginPacket(0x101);

  for (int i = 0; i < 4; i++)
  {
    CAN.write((uint8_t)(motor[i] >> 8));
    CAN.write((uint8_t)(motor[i] & 0xFF));
  }

  CAN.endPacket();

  int packetSize = CAN.parsePacket();
  uint8_t rx[8] = {0};
  if (packetSize == 8 && CAN.packetId() == 0x100)
  {
    for (int i = 0; i < 8; i++)
    {
      rx[i] = CAN.read();
    }
  }

  int32_t count_1 =
      ((int32_t)rx[0] << 24) |
      ((int32_t)rx[1] << 16) |
      ((int32_t)rx[2] << 8) |
      ((int32_t)rx[3]);

  int32_t count_2 =
      ((int32_t)rx[4] << 24) |
      ((int32_t)rx[5] << 16) |
      ((int32_t)rx[6] << 8) |
      ((int32_t)rx[7]);

  static uint32_t last = 0;

  if (millis() - last > 100)
  {
    last = millis();

    printf("%ld %ld\n", count_1, count_2);
  }
}