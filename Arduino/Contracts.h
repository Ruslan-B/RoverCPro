#ifndef _CONTRACTS_H_
#define _CONTRACTS_H_

const int UDP_PORT = 32768;

const uint8_t HEADER_ID = 'R';

enum PayloadType
{
  None = 0,
  Ack = 1,
  Move = 2
};

struct PackerHeader
{
  uint8_t Id;
  PayloadType Type;
  uint8_t PayloadSize;
};

struct RoverMove
{
  int8_t MoveX;
  int8_t MoveY;
  int8_t Rotation;
  bool Grip;
};
#endif
