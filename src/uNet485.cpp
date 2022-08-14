
#include "uNet485.h"
//#define DEBUGINPUT
//#define DEBUGOUTPUT
uNet485::uNet485()
{

}

void uNet485::begin(Stream* serialPort, int xmitEnable)
{
	_serialport = serialPort;
	_xmitEnable = xmitEnable;
	_rs485Mode = xmitEnable > 0 ? true : false;
#ifdef DEBUGINPUT
	_serialport->print("In uNet485.  The selected Xmit pin is: ");
	_serialport->println(_xmitEnable);
#endif
	if(_rs485Mode) pinMode(_xmitEnable, OUTPUT);
}

unsigned char* uNet485::getData()
{
#ifdef DEBUGINPUT
	Serial1.println("uNet485::getData()");
	Serial1.println(TransferBuffer[9]);
#endif

	return TransferBuffer;
}

unsigned char getChecksum()
{
	return Checksum;
}

boolean uNet485::readPort()
{
	static boolean packetInFlight;
	static char lastChar;
	boolean result = false;
	static int index;
	char inChar;

	while (_serialport->available())
	{
		// get the next byte:
		inChar = (char)_serialport->read();
		if (inChar == STX && !packetInFlight)
		{
			index = 0;
#ifdef DEBUGINPUT
			Serial1.println("STX received");
#endif
			// STX detected
			packetInFlight = true;
			_inFrame.initBuffer();
			_inFrame.setStart(inChar);
		}
		else if (inChar == ETX && index + 2 >= FRAME_SIZE)
		{
			// ETX and end of message detected.  The count is used in case the packet contains a 0x03 as data
			packetInFlight = false;
#ifdef DEBUGINPUT
			Serial1.println("ETX received");
#endif
			_inFrame.setChecksum(lastChar);
#ifdef DEBUGINPUT
			Serial1.print("Global buffer content: ");
			Serial1.println((char*)TransferBuffer);
#endif
			memcpy((char*)TransferBuffer, (const char*)_inFrame.getData(), FRAMEPACKET_SIZE);
			Checksum = _inFrame.getChecksum();
#ifdef DEBUGINPUT
			Serial1.print("Global buffer content: ");
			Serial1.println((char*)TransferBuffer);
#endif
			result = true;
		}
		else
		{
#ifdef DEBUGINPUT
			Serial1.write(inChar);
#endif
			_inFrame.setChar(inChar);
			lastChar = inChar;
			index++;
		}
	}
	return result;
}

boolean uNet485::writePort(unsigned char* data)
{
	_outFrame.setPacket((unsigned char*)data);

#ifdef DEBUGOUTPUT
	byte* x = (byte*)_outFrame.getFrame();

	for (int i = 0; i < FRAME_SIZE; i++)
	{
		Serial1.print(x[i], HEX);
	}
	Serial1.println();

	if(_rs485Mode) digitalWrite(_xmitEnable, HIGH);
	_serialport->write((byte*)x, FRAME_SIZE);
	_serialport->flush();
	if (_rs485Mode) digitalWrite(_xmitEnable, LOW);
#else
	if (_rs485Mode) digitalWrite(_xmitEnable, HIGH);
	_serialport->write((byte*)_outFrame.getFrame(), FRAME_SIZE);
	_serialport->flush();
	if (_rs485Mode) digitalWrite(_xmitEnable, LOW);
#endif

	return true;
}
