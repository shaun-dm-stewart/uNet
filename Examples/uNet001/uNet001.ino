

uNetSocket _socket;
char _inputString[64];
bool _stringComplete = false;
bool _incomingData = false;

void dataReceived(unsigned char* data)
{
  Serial1.print("uNet.ino received: ");
  Serial1.println((char*)data);
}

void dataSent()
{
}

bool getRxData()
{
  while (Serial1.available())
  {
    // get the next byte:
    char inChar = (char)Serial1.read();
    if (inChar == STX)
    {
      _stringComplete = false;
      // There's a fresh data stream on its way
      _incomingData = true;
      //Clear down the buffer, there is new data waiting.
      _inputString[0] = '\0';
    }
    else
    {
      // it's not STX so process accordingly
      if (_incomingData)
      {
        // We are reading a data stream
        if (inChar == ETX)
        {
          //_serialport->println("ETX detected");
          // End Transmission detected. Set a flag
          // so the main loop can do something about it:
          _incomingData = false;
          _stringComplete = true;
        }
        else
        {
          int index = strlen(_inputString);
          // Keep building the incoming data stream until the buffer is full
          if (index < 64)
          {
            _inputString[index] = inChar;
            _inputString[index + 1] = '\0';
          }
          else
          {
            _incomingData = false;
            _stringComplete = true;
          }
        }
      }
    }
  }

  if (_stringComplete)
  {
    _stringComplete = false;
    return true;
  }
  else return false;
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  _socket.begin(&Serial, dataReceived, dataSent, 4, 2);
}

// the loop function runs over and over again until power down or reset
void loop() {

  if (_socket.readPacket())
  {
  }
  if (getRxData())
  {
    _socket.writePacket(1, (unsigned char*)_inputString);
  }
}
