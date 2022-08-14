// Frame.h

#ifndef _FRAME_h
#define _FRAME_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Definitions.h"

class Frame
{
public:
	Frame();
	void initBuffer();
	void setStart(unsigned char start);
	void setEnd(unsigned char end);
	void setChecksum(unsigned char checksum);
	void setChar(unsigned char dataChar);
	void setPacket(unsigned char* data);
	unsigned char getChecksum();
	unsigned char* getFrame();
	unsigned char calculateChecksum();
	unsigned char* getData();
private:
	struct FramePacket
	{
		unsigned char stx;
		unsigned char packet[FRAMEPACKET_SIZE];
		unsigned char checksum;
		unsigned char etx;
	};

	union RawFrame
	{
		unsigned char framebuffer[FRAME_SIZE];
		FramePacket frameSegment;
	};

	int bufferIndex;
	RawFrame rawFrame;
};

#endif
