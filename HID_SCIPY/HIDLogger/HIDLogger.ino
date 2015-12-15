
//---------------------------------------------------------------------------------------------------------------------
// INITIALIZATIONS PARAMETERS
//---------------------------------------------------------------------------------------------------------------------
#include <ADC.h>

ADC *adc = new ADC();     // adc object for first pair of microphones
ADC::Sync_result result;
IntervalTimer myTimer;    // interrupt library for Teensy3.1

#define REAL               // REAL, FAKE, TIMING; obtains analog data or creates a fake set of signals with offset in one
#define HIDSEND            // SERIALSEND or HIDSEND; chooses which method to send the buffers out via, serial just for test

const int FFTSIZE = 1024;    // can't change without changing other parts of HID code and Python processing code
const int RESOLUTION = 12;   // (8-13) resolution to sample each ADC at
const int AVERAGES = 1;      // (1-...) number of analog samples to average into one data point in the buffer
const int INTERRUPT = 20;    // acquisition interrupt timing in usecs, x > 17 (50 = 0.00005secs = 20 kHz, 20 = 50 KhZ)
const int DELAY = 5;         // delay between one set of acquisitions and the next, probably goes to zero or small number
const int BLINKS = 11;       // number of times to flash the LED 
const int DURATION = 1000;   // time (msecs) to flash LED


// Clockwise, looking down at the board, starting at microphone that was labeled "A2". Left gaps in naming in 
// case all 8 microphones are necessary as opposed to the 4 we are currently using.  Currently it is sampling 
// (A and C) simultaneously and then (E and G) simultaneously. 
// Pin to ADC mapping: https://forum.pjrc.com/attachment.php?attachmentid=2881 
#define microphoneA A1  // ADC 0
#define microphoneE A2  // ADC 1: was A3, changed with board rev 9/7/15
#define microphoneC A0  // ADC 0
#define microphoneG A3  // ADC 1: was A2, changed with board rev 9/7/15 

#define LEDPIN 13

volatile int16_t signal1[FFTSIZE], signal2[FFTSIZE], signal3[FFTSIZE], signal4[FFTSIZE];
volatile int16_t sampleCounter = 0;

const int mainPacketCount = 2048;
byte buffer[64];              // RawHID packets are always 64 bytes
unsigned int packetCount = 0;
int n;                        // counter, moved out of loop() to create separate function assembleAndSend

long startTime;  // timing loop variables
long stopTime;
long totalTime;

const int HID_TIMEOUT = 50;

//---------------------------------------------------------------------------------------------------------------------
// SET-UP
//---------------------------------------------------------------------------------------------------------------------
void setup() {
  
  Serial.begin(115200);
  delay(100);
  pinMode(microphoneA, INPUT); 
  pinMode(microphoneE, INPUT); 
  pinMode(microphoneC, INPUT); 
  pinMode(microphoneG, INPUT);
  pinMode(LEDPIN, OUTPUT);

  //----------------SET-UP FOR FIRST PAIR OF SIGNALS TO READ SIMULTANEOUSLY--------------------------------------------
  // ADC0
  // all parameters documented thoroughly inside ADC and ADC_module library files
  adc->setAveraging(AVERAGES); // set number of samples to average into one data point
  adc->setResolution(RESOLUTION); // set bits of resolution
  adc->setConversionSpeed(ADC_VERY_HIGH_SPEED); // change the conversion speed
  adc->setSamplingSpeed(ADC_VERY_HIGH_SPEED); // change the sampling speed

  //ADC1
  adc->setAveraging(AVERAGES, ADC_1); // set number of samples to average into one data point
  adc->setResolution(RESOLUTION, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_VERY_HIGH_SPEED, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_VERY_HIGH_SPEED, ADC_1); // change the sampling speed
      
  delay(1000);
  Serial.println("end of setup");

  statusLED(BLINKS,DURATION);  // flash LED to show signs of life
  
  #ifdef REAL  
    myTimer.begin(sampleAndBuffer, INTERRUPT);  // acquisition interrupt timing, microseconds (50 = 0.00005 secs = 20 kHz)
  #endif
  delay(200);
}

//---------------------------------------------------------------------------------------------------------------------
// LOOP
//---------------------------------------------------------------------------------------------------------------------
void loop()  {
  
//  Serial.println(" BEGIN ");
//  packetCount = 0;

  
#ifdef REAL
  // Once the buffers are full (@ the FFTSIZE), turn off interrupts, run the code to assemble and 
  // send values over HID, reset the sampleCounter variable to zero, run the delay loop, and then 
  // turn interrupts on again, to begin collecting another frame of buffer's worth of data.
  if (sampleCounter >= FFTSIZE)  {
     noInterrupts();
     
     #ifdef SERIALSEND
       sendViaSerial();
     #endif
     
     #ifdef HIDSEND
       sendViaHID();
     #endif
     
     sampleCounter = 0;
     
     toggleLED();  // flash LED to show signs of life
     delay(DELAY);
     interrupts();     
  }

#endif

#ifdef FAKE
// Generate two signals, one with a given offset (specified below) from the other to be detected in 
// phase offset calcs
//unsigned offset = random(FFTSIZE / 4);
  unsigned offset = 100;
  for (unsigned i = 0; i < FFTSIZE + offset; i++) {
    int16_t val = ((int16_t) random(65536));
    if (i < FFTSIZE)
      signal1[i] = val;
    if (i > offset)
      signal2[i - offset] = val;
  }

     #ifdef SERIALSEND
       sendViaSerial();
     #endif
     
     #ifdef HIDSEND
       sendViaHID();
     #endif    
    
    delay(DELAY);
    
#endif

#ifdef TIMING
  startTime = millis();
  for (int i = 0; i < 10000; i++)  {
    sampleAndBuffer();
    if (sampleCounter == FFTSIZE)  {
      sampleCounter = 0;
    }
  }
  stopTime = millis();
  totalTime = stopTime - startTime;
  Serial.print("elapsed time for 10,000 loops (sample four signals, error check, write to buffers = ");
  Serial.print(totalTime);  Serial.println(" msecs");
  Serial.print("time for one loop = "); Serial.print((float)totalTime/10000*1000); Serial.println(" microseconds");
  Serial.print("example data point = "); Serial.println(signal1[100]);
  Serial.println("");
#endif
}

//---------------------------------------------------------------------------------------------------------------------
// FUNCTIONS
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
// called via Interrupt, samples the microphone channels and puts values in individual buffers
// in testing, this function completes in 16.3 microseconds
void sampleAndBuffer(void)  {
  
  if (sampleCounter < FFTSIZE) {  // if sampleCounter is at FFTSIZE, do nothing in interrupt until buffer is empty
   // Acquire first pair of microphone readings simultaneously and write to signal buffers
   result = adc->analogSynchronizedRead(microphoneA,microphoneE);
   signal1[sampleCounter] = (uint16_t)result.result_adc0;
   signal2[sampleCounter] = (uint16_t)result.result_adc1;
   
   // Acquire second pair of microphone readings simultaneously and write to signal buffers
   result = adc->analogSynchronizedRead(microphoneC,microphoneG);
   signal3[sampleCounter] = (uint16_t)result.result_adc0;
   signal4[sampleCounter] = (uint16_t)result.result_adc1;

   // Increment the buffer position counter
   sampleCounter++;
  }
}

//---------------------------------------------------------------------------------------------------------------------
// for testing purposes, outputs signal buffers via Serial
void sendViaSerial()  {
  
  Serial.println("");
  Serial.println("signal1 =");
  for (int i = 0; i < FFTSIZE; i++)  {
    Serial.print(signal1[i]);  Serial.print(",");
  }
  Serial.println(" ");
  
  Serial.println("signal2 =");
  for (int i = 0; i < FFTSIZE; i++)  {
    Serial.print(signal2[i]);  Serial.print(",");
  }
  Serial.println(" ");

  Serial.println("signal3 =");
   for (int i = 0; i < FFTSIZE; i++)  {
     Serial.print(signal3[i]);  Serial.print(",");
   }
   Serial.println(" ");
 
   Serial.println("signal4 =");
   for (int i = 0; i < FFTSIZE; i++)  {
     Serial.print(signal4[i]);  Serial.print(",");
   }
   Serial.println(" ");
 
}

//---------------------------------------------------------------------------------------------------------------------
// for actual use, outputs signal buffers via HID interface.  ifdef is to prevent errors when in Serial output mode


#ifdef HIDSEND
void sendViaHID()  {

  packetCount = 0;
  
  int i = 0;
  int j = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 1st frame
  
  while ( i < mainPacketCount )
  {
    for( int k = 0; k < 32; k++ )
    {
      buffer[k * 2] = highByte(signal1[j]);
      buffer[k * 2 + 1] = lowByte(signal1[j]);
      j++;
    }

    n = RawHID.send(buffer, HID_TIMEOUT);
    packetCount++;
    i += 64;
  }

  // this is the end of a frame, build all zeros except the last 4 values
  for ( int k = 0; k < 60; k++)
  {
    buffer[k] = 0x00;
  }

  buffer[60] = 0xFF;
  buffer[61] = 0xFF;
  buffer[62] = 0xFF;
  buffer[63] = 0xFF;

  n = RawHID.send(buffer, HID_TIMEOUT);
  packetCount++;

  // reset
  i = 0;
  j = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2nd frame

  while ( i < mainPacketCount )
  {
    for( int k = 0; k < 32; k++ )
    {
      buffer[k * 2] = highByte(signal2[j]);
      buffer[k * 2 + 1] = lowByte(signal2[j]);
      j++;
    }

    n = RawHID.send(buffer, HID_TIMEOUT);
    packetCount++;

    i += 64;
  }

   // this is the end of a frame, build all zeros except the last 4 values
  for ( int k = 0; k < 60; k++)
  {
    buffer[k] = 0x00;
  }

  buffer[60] = 0xFF;
  buffer[61] = 0xFF;
  buffer[62] = 0xFF;
  buffer[63] = 0xFF;

  n = RawHID.send(buffer, HID_TIMEOUT);
  packetCount++;

  i = 0;
  j = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3rd frame

  while ( i < mainPacketCount )
  {
    for( int k = 0; k < 32; k++ )
    {
      buffer[k * 2] = highByte(signal3[j]);
      buffer[k * 2 + 1] = lowByte(signal3[j]);
      j++;
    }

    n = RawHID.send(buffer, HID_TIMEOUT);
    packetCount++;

    i += 64;
  }


  // this is the end of a frame, build all zeros except the last 4 values
  for ( int k = 0; k < 60; k++)
  {
    buffer[k] = 0x00;
  }

  buffer[60] = 0xFF;
  buffer[61] = 0xFF;
  buffer[62] = 0xFF;
  buffer[63] = 0xFF;

  RawHID.send(buffer, HID_TIMEOUT);
  packetCount++;

  i = 0;
  j = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 4th frame

  while ( i < mainPacketCount )
  {
    for( int k = 0; k < 32; k++ )
    {
      buffer[k * 2] = highByte(signal4[j]);
      buffer[k * 2 + 1] = lowByte(signal4[j]);
      j++;
    }

    n = RawHID.send(buffer, HID_TIMEOUT);
    packetCount++;

    i += 64;
  }
  
  // this is the end of the 4th frame, build all zeros except the last 4 values
  for ( int k = 0; k < 56; k++)
  {
    buffer[k] = 0x00;
  }
  buffer[56] = 0xFF;
  buffer[57] = 0xFF;
  buffer[58] = 0xFF;
  buffer[59] = 0xFF;
  buffer[60] = 0xFF;
  buffer[61] = 0xFF;
  buffer[62] = 0xFF;
  buffer[63] = 0xFF;

  n = RawHID.send(buffer, HID_TIMEOUT);
  packetCount++;

  Serial.print(" OK ");
  Serial.println(packetCount);
}

#endif

//---------------------------------------------------------------------------------------------------------------------
// blinking LEDs
void statusLED (int blinks, int duration)  {
  int temporary = duration/blinks;
  for (int i = 0; i < blinks; i++)  {
    toggleLED();
    delay((temporary));
    }
}

void toggleLED ()  {
  digitalWriteFast(LEDPIN,!digitalReadFast(LEDPIN));
}

