#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H
constexpr unsigned char NUL = 0x00;
constexpr unsigned char SOH = 0x01;
constexpr unsigned char STX = 0x02;
constexpr unsigned char ETX = 0x03;
constexpr unsigned char EOT = 0x04;
constexpr unsigned char ENQ = 0x05;
constexpr unsigned char ACK = 0x06;
constexpr unsigned char DC1 = 0x11;

constexpr int FRAME_SIZE = 48;
constexpr int FRAMEOVERHEADS = 3;
constexpr int FRAMEPACKET_SIZE = FRAME_SIZE - FRAMEOVERHEADS;

static unsigned char TransferBuffer[64];
static unsigned char Checksum;

#endif

