// 
// 
// 

//#define DEBUGIN

#include "Frame.h"

Frame::Frame()
{
	Frame::bufferIndex = 0;
	rawFrame.frameSegment.checksum = 0x00;
}

void Frame::setStart(unsigned char start)
{
	rawFrame.frameSegment.stx = start;
}

void Frame::setEnd(unsigned char end)
{
	rawFrame.frameSegment.etx = end;
}

void Frame::setChecksum(unsigned char checksum)
{
	rawFrame.frameSegment.checksum = checksum;
}

void Frame::initBuffer()
{
	bufferIndex = 0;
	memset(rawFrame.framebuffer, 0x00, FRAME_SIZE);
}

void Frame::setChar(unsigned char dataChar)
{
	rawFrame.frameSegment.packet[bufferIndex] = dataChar;
	bufferIndex++;
}

void Frame::setPacket(unsigned char* data)
{
	rawFrame.frameSegment.stx = STX;
	rawFrame.frameSegment.etx = ETX;
	memcpy((char*)rawFrame.frameSegment.packet, (const char*)data, FRAMEPACKET_SIZE);
}

unsigned char* Frame::getFrame()
{

	rawFrame.frameSegment.checksum = calculateChecksum();
	return rawFrame.framebuffer;
}

unsigned char Frame::calculateChecksum()
{
	char chkSumByte = 0x00;
	for (int i = 0; i < FRAMEPACKET_SIZE; i++)
		chkSumByte ^= rawFrame.frameSegment.packet[i];
	return chkSumByte;
}

unsigned char* Frame::getData()
{
#ifdef DEBUGIN
	Serial1.print("Frame::getData: ");
	Serial1.println((char*)rawFrame.frameSegment.packet);

#endif

	return rawFrame.frameSegment.packet;
}

unsigned char Frame::getChecksum()
{
	return rawFrame.frameSegment.checksum;
}
