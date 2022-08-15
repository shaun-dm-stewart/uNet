/*
 Name:		uNet.cpp
 Created:	8/31/2020 10:58:58 AM
 Author:	Shaun
 Editor:	http://www.visualmicro.com
*/

#include "uNet.h"

//#define DEBUGIN
//#define DEBUG
//#define DEBUGOUT

uNetSocket::uNetSocket()
{

}

void uNetSocket::begin(Stream* serialPort, DataReceived received, DataTransmitted sent, int nodeAddress, int xmitEnable)
{
#ifdef DEBUG
	Serial1.println("Starting ...");
#endif
	_serialPort = serialPort;
	_nodeAddress = (unsigned char)nodeAddress;
	_frameLayer.begin(serialPort, xmitEnable);
	_received = received;
	_sent = sent;
}

bool uNetSocket::readPacket()
{
	if (_frameLayer.readPort())
	{
#ifdef DEBUGIN
		Serial1.println("uNetSocket::readPacket");
#endif
		memcpy(_inbound.Buffer, _frameLayer.getData(), FRAMEPACKET_SIZE);

		int dataSize = MAX_FILL;
		// Reject if the packet is not for this node
#ifdef DEBUGIN
		Serial1.print("Socket buffer: ");
		Serial1.println((char*)_inbound.Data.data);
		Serial1.print("Node Address: ");
		Serial1.println(_nodeAddress);
		Serial1.print("Inbound Address: ");
		Serial1.println(_inbound.Data.destination);

#endif
		if (_inbound.Data.destination != _nodeAddress && _inbound.Data.destination != BROADCAST)
		{
#ifdef DEBUGIN
			Serial1.println("Wrong node address");
#endif
			return false;
		}
		// The packet is for this node
		int msgType = ExtractMessageType(_inbound.Data);
#ifdef DEBUGIN
		Serial1.print("Message type extracted: ");
		Serial1.println(msgType);
#endif
		switch (msgType)
		{
		case 1:
			// Received an ACK. Send another packet if necessary
#ifdef DEBUGOUT
			Serial1.println("ACK received: ");
#endif
			_packetIndex++;  // The first data containing packet is at index 1 not 0

			if (_packetIndex < _totalPackets)
			{
#ifdef DEBUGOUT
				Serial1.print("Next output: ");
				Serial1.println((char*)_ioBuffer.oBuffer[_packetIndex].Data.data);
#endif
				Transmit(_ioBuffer.oBuffer[_packetIndex]);
			}
			else
			{
				_sent(); //The callback function to alert the caller that the packet has been sent
			}
			break;
		case 2:
			// Received an EOT so the message is complete
#ifdef DEBUGIN
			Serial1.println("EOT received: ");
#endif
			_received(_rxBuffer,(int)_inbound.Data.source);
			break;
		case 3:
#ifdef DEBUGIN
			Serial1.println("SOH received: ");
#endif
			// Received an SOH so it's a new message
			initialiseRXBuffer();
			break;
		case 4:
			// It's a continuation
#ifdef DEBUGIN
			Serial1.print("It's a continuation: ");
			Serial1.println((char*)_inbound.Data.data);
#endif
			strncat((char*)_rxBuffer, (const char*)_inbound.Data.data, MAX_FILL);
			_offset += MAX_FILL;
#ifdef DEBUGIN
			Serial1.print("_rxBuffer: ");
			Serial1.write((char*)_rxBuffer);
			Serial1.println();
#endif
			break;

		}
	}
}

bool uNetSocket::writePacket(int destinationNode, unsigned char* data)
{
#if DEBUGOUT
	Serial1.println("write Packet:");
#endif
	int packetLength = strlen((const char*)data);
	int pSize = FRAMEPACKET_SIZE - 2;
	int numPackets = (packetLength / pSize) + (packetLength % pSize > 0 ? 1 : 0);
	int index = 0;

	// we will assume it is less than 240 bytes
	initialiseTXBuffer();

	_ioBuffer.oBuffer[0].Data.destination = destinationNode;
	_ioBuffer.oBuffer[0].Data.source = _nodeAddress;
	_ioBuffer.oBuffer[0].Data.data[0] = SOH;
#if DEBUGOUT	
	Serial1.println("Building output buffers");
	Serial1.println(packetLength);
#endif
	for (int i = 0; i < numPackets; i++)
	{
		_ioBuffer.oBuffer[i + 1].Data.destination = destinationNode;
		_ioBuffer.oBuffer[i + 1].Data.source = _nodeAddress;

		int biteSize;
		if (packetLength - index > pSize)
		{
			biteSize = pSize;
		}
		else biteSize = (packetLength - index);

		memcpy(&_ioBuffer.oBuffer[i + 1].Data.data, &data[index], biteSize);
#if DEBUGOUT
		Serial1.write((char*)_ioBuffer.oBuffer[i].Data.data, biteSize);
		Serial1.println();
#endif
		index += biteSize;
	}

	_ioBuffer.oBuffer[numPackets + 1].Data.destination = destinationNode;
	_ioBuffer.oBuffer[numPackets + 1].Data.source = _nodeAddress;
	_ioBuffer.oBuffer[numPackets + 1].Data.data[0] = EOT;
	_totalPackets = numPackets + 2;

#if DEBUGOUT
	Serial1.println("Total Packets: ");
	Serial1.println(_totalPackets);
#endif
	Transmit(_ioBuffer.oBuffer[_packetIndex]);

	return true;
}

int uNetSocket::ExtractMessageType(Packet data)
{
	int result = -1;
	char msgType = data.data[0];
	if (msgType == ACK)
	{
		// It's from the node I am transmitting to and it has acknowledged receipt of a packet
		result = 1;
	}
	else if (msgType == EOT)
	{
		// It's an emd of transmission
		result = 2;
	}
	else if (msgType == SOH)
	{
		// It's a new message
		result = 3;
	}
	else
	{
		// It's a continuation
		result = 4;
	}

	// Send an ack if called for
	if (result > 1)
	{
		DataPacket outAck;
		memset(outAck.Buffer, 0x00, FRAME_SIZE);
		outAck.Data.destination = data.source;
		outAck.Data.source = data.destination;
		outAck.Data.data[0] = ACK;
		Transmit(outAck);
	}

	return result;
}

void uNetSocket::Transmit(DataPacket data)
{
#ifdef DEBUGOUT
	Serial1.print("Package to send: ");
	Serial1.println((char*)data.Buffer);
	Serial1.println((char*)data.Data.data);
#endif
	_frameLayer.writePort(data.Buffer);
}

void uNetSocket::initialiseTXBuffer()
{
	_packetIndex = 0;
	memset(_ioBuffer.oBuffer, 0x0, (FRAMEPACKET_SIZE * 5));
}

void uNetSocket::initialiseRXBuffer()
{
	_offset = 0;
	memset(_rxBuffer, 0x0, (FRAMEPACKET_SIZE * 5));
}


