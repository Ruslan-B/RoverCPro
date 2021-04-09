#include "RoverC.h"

void SetChargingCurrent(uint8_t CurrentLevel)
{
  Wire1.beginTransmission(0x34);
  Wire1.write(0x33);
  Wire1.write(0xC0 | (CurrentLevel & 0x0f));
  Wire1.endTransmission();
}

void RoverC_Init(void)    //sda  0     scl  26
{
  Wire.begin(0, 26, 1000);

  SetChargingCurrent(4);
}

void Send_iic(uint8_t Register, uint8_t Speed)
{
  Wire.beginTransmission(ROVER_ADDRESS);
  Wire.write(Register);
  Wire.write(Speed);
  Wire.endTransmission();
}

void Move_forward(int8_t Speed)
{
  Send_iic(0x00, Speed );
  Send_iic(0x01, Speed );
  Send_iic(0x02, Speed );
  Send_iic(0x03, Speed );
}

void Move_back(int8_t Speed)
{
  Send_iic(0x00, (-1) * Speed );
  Send_iic(0x01, (-1) * Speed );
  Send_iic(0x02, (-1) * Speed );
  Send_iic(0x03, (-1) * Speed );
}

void Move_turnleft(int8_t Speed)
{
  Send_iic(0x00, Speed );
  Send_iic(0x01, (-1) * Speed );
  Send_iic(0x02, Speed );
  Send_iic(0x03, (-1) * Speed );
}

void Move_turnright(int8_t Speed)
{
  Send_iic(0x00, (-1) * Speed );
  Send_iic(0x01, Speed );
  Send_iic(0x02, (-1) * Speed );
  Send_iic(0x03, Speed );
}

void Move_left(int8_t Speed)
{
  Send_iic(0x00, (-1) * Speed );
  Send_iic(0x01, Speed );
  Send_iic(0x02, Speed );
  Send_iic(0x03, (-1) * Speed );
}

void Move_right(int8_t Speed)
{
  Send_iic(0x00, Speed );
  Send_iic(0x01, (-1) * Speed );
  Send_iic(0x02, (-1) * Speed );
  Send_iic(0x03, Speed );
}
void Move_stop(int8_t Speed)
{
  Send_iic(0x00, 0 );
  Send_iic(0x01, 0 );
  Send_iic(0x02, 0 );
  Send_iic(0x03, 0 );
}

void RoverC_Move(int16_t Vtx, int16_t Vty, int16_t Wt)
{
  Wt = (Wt > 100) ? 100 : Wt;
  Wt = (Wt < -100) ? -100 : Wt;

  Vtx = (Vtx > 100) ? 100 : Vtx;
  Vtx = (Vtx < -100) ? -100 : Vtx;
  Vty = (Vty > 100) ? 100 : Vty;
  Vty = (Vty < -100) ? -100 : Vty;

  Vtx = (Wt != 0) ? Vtx * (100 - abs(Wt)) / 100 : Vtx;
  Vty = (Wt != 0) ? Vty * (100 - abs(Wt)) / 100 : Vty;

  Send_iic(0x00, Vty - Vtx - Wt);
  Send_iic(0x01, Vty + Vtx + Wt);
  Send_iic(0x02, Vty - Vtx + Wt);
  Send_iic(0x03, Vty + Vtx - Wt);
}

void Servo_angle(uint8_t Servo_ch, uint8_t degree)
{
  degree = min(90, int(degree));
  degree = max(0, int(degree));
  Send_iic((Servo_ch - 1) | 0x10 , int(degree));
}

void Servo_pulse(uint8_t Servo_ch, uint16_t width)
{
  width = min(2500, int(width));
  width = max(500, int(width));
  Send_iic((Servo_ch - 1) | 0x20, width);
}
