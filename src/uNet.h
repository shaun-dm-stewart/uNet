/*
 Name:		uNet.h
 Created:	8/31/2020 10:58:58 AM
 Author:	Shaun
 Editor:	http://www.visualmicro.com
*/

#ifndef _uNet_h
#define _uNet_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "uNet485.h"

constexpr int MAX_FILL = FRAMEPACKET_SIZE - 2;
constexpr int RXBUFFER_SIZE = 240;
constexpr int TXBUFFER_COUNT = 5;
constexpr int BROADCAST = -1;

class uNetSocket
{
public:
	typedef void (*DataReceived) (unsigned char* data);
	typedef void (*DataTransmitted)();
	uNetSocket();
	void begin(Stream* serialPort, DataReceived received, DataTransmitted sent, int nodeAddress, int xmitEnable);
	bool readPacket();
	bool writePacket(int destinationNode, unsigned char* data);

private:
	typedef struct Packet
	{
		unsigned char destination;
		unsigned char source;
		unsigned char data[MAX_FILL];
	};

	union DataPacket
	{
		Packet Data;
		unsigned char Buffer[MAX_FILL];
	};

	typedef struct IOBuffer
	{
		DataPacket oBuffer[TXBUFFER_COUNT];
	};

	void Transmit(DataPacket data);
	void initialiseTXBuffer();
	void initialiseRXBuffer();
	int ExtractMessageType(Packet packet);
	uNet485 _frameLayer;
	DataReceived _received;
	DataTransmitted _sent;
	unsigned char _rxBuffer[RXBUFFER_SIZE];
	unsigned char _nodeAddress;
	Stream* _serialPort;
	DataPacket _inbound;
	int _packetIndex;
	int _totalPackets;
	int _offset = 0;
	IOBuffer _ioBuffer;
};

#endif

