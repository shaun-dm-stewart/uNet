
#ifndef _UNET485_h
#define _UNET485_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Definitions.h"
#include "Frame.h"

class uNet485
{
public:
	uNet485();
	void begin(Stream* serialPort, int xmitEnable);
	boolean readPort();
	boolean writePort(unsigned char* data);
	unsigned char* getData();
	unsigned char getChecksum();
private:
	Stream* _serialport = nullptr;
	int _xmitEnable;
	bool _rs485Mode;
	Frame _inFrame;
	Frame _outFrame;
};

#endif
