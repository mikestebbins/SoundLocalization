byte buffer[64];  
int n;
byte j = 0x00;
byte jNew;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  
jNew = sendViaHID(j);
j = jNew;
delay(1000);

}

byte sendViaHID(byte j)  {

byte temp = j;

for (int i = 0; i < 56; i++)  {
  buffer[i] = temp;
  temp++;
  }

  // this is the end of a frame
  buffer[56] = 0xFF;
  buffer[57] = 0xFF;
  buffer[58] = 0xFF;
  buffer[59] = 0xFF;
  buffer[60] = 0xFF;
  buffer[61] = 0xFF;
  buffer[62] = 0xFF;
  buffer[63] = 0xFF;
  
  n = RawHID.send(buffer, 10);

  return temp;
}

