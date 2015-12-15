volatile long milliTimeout;
volatile int firstPin;
volatile int lastPin;
String readOut  = "null";
const int TIMEOUT = 50000;

void setup() {
  Serial.begin(115200);
  Serial.println(" OK " );
  
  //interrupts for directional mic
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);

  attachInterrupt(2, two, RISING);
  attachInterrupt(3, three, RISING);
  attachInterrupt(4, four, RISING);
  attachInterrupt(5, five, RISING);
  attachInterrupt(6, six, RISING);
  attachInterrupt(7, seven, RISING);
  attachInterrupt(8, eight, RISING);
  attachInterrupt(9, nine, RISING);
}



void loop() {

    //digitalWrite(pin, state);
  if(milliTimeout > 0)
  {
    milliTimeout -= 1;
  }

  if(firstPin != lastPin && firstPin != 0)
    {
      switch (firstPin){
        case 2:
          readOut = "1 orange";
          break;
        case 3:
          readOut = "2 yellow";
          break;
        case 7:
          readOut = "3 green";
          break;
        case 9:
          readOut = "4 blue";
          break;
        case 4:
          readOut = "5 violet";
          break;
        case 5:
          readOut = "6 gray";
          break;
        case 6:
          readOut = "7 white";
          break;
        case 8:
          readOut = "8 black";
          break;
             
      }
    
    
    Serial.println (readOut);
    lastPin = firstPin; // cache for next time so we don't send again
    firstPin = 0;
  }
  
}

///////////////////////////////////////////////////////////////////////////////
// interrupts for the directional mic
///////////////////////////////////////////////////////////////////////////////

void two()
{ 
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 2;
  }
}

void three()
{ 
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 3;
  }
}

void four()
{  
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 4;
  }
}

void five()
{
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 5;
  }
}

void six()
{  
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 6;
  }
}

void seven()
{
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 7;
  }
}

void eight()
{
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 8;
  }
}

void nine()
{
  if(milliTimeout < 1)
  {
    milliTimeout = TIMEOUT;
    firstPin = 9;
  }
}


