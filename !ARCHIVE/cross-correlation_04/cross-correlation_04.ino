/* Mike Stebbins 07-04-2015
Timed buffer fill loop for two analog reads to determine sampling rate for FFT work.
time for 1000 samples, in millis = 42, (including delayMicroseconds(23))
= 0.042 msecs / sample (group of 2 samples obtained concurrently)
= 23.81 kHz / sample rate
*/

/* Kire Pudsje, Feb. 2015 */

#include "arm_math.h"
#define FFTSIZE 1024

int analogPin2 = A2;
int analogPin4 = A4;
int ledPin = 13;
//int timingLoopSize = 1000;

//int16_t fft_buf[FFTSIZE * 2];
int16_t fft_buf[FFTSIZE];
int16_t samples[2*FFTSIZE];
int16_t magnitudes[FFTSIZE];

//int16_t s1[FFTSIZE], s2[FFTSIZE];
//pre-defined arrays to use for FFT/cross correlation testing, comment out otherwise
int16_t s1[]={512,538,564,589,613,636,658,679,697,713,728,740,750,758,764,767,769,769,768,765,762,757,752,747,742,737,733,730,728,727,728,730,734,740,747,757,768,781,795,810,827,844,862,881,899,917,934,951,966,980,993,1003,1011,1018,1021,1023,1022,1019,1013,1005,995,983,970,955,939,921,903,885,867,849,831,814,799,784,771,759,750,741,735,731,728,727,727,729,732,736,741,746,751,756,761,765,768,769,770,768,765,760,752,743,731,717,701,683,664,642,619,595,570,544,518,492,466,440,416,392,370,349,331,314,299,286,275,267,261,256,254,253,254,257,260,265,269,275,280,285,289,292,295,296,296,294,290,285,278,269,258,246,232,217,200,183,165,147,129,111,93,76,60,46,33,22,13,7,2,0,0,3,8,16,25,36,50,64,80,97,115,133,152,170,188,205,221,235,249,261,271,280,286,291,294,296,296,294,292,288,283,278,273,268,263,259,256,254,253,254,257,262,269,278,289,302,318,335,354,375,398,422,447,472,498,525,551,576,601,625,648,669,688,705,721,734,745,754,761,766,769,770,769,767,764,760,755,750,745,740,735,731,729,727,727,729,732,737,743,752,762,774,788,802,818,835,853,871,890,908,926,943,959,973,987,998,1007,1015,1020,1023,1023,1021,1016,1010,1001,990,977,963,947,930,912,894,876,858,840,823,806,791,777,765,754,745,738,733,729,727,727,728,731,734,738,743,748,754,758,763,766,769,770,769,767,762,756,748,737,724,709,692,674,653,631,607,583,557,531,505,479,453,428,404,381,359,340,322,306,292,280,271,263,258,255,253,254,255,258,262,267,272,277,282,287,291,294,296,296,295,292,288,282,273,264,252,239,224,209,192,174,156,138,120,102,84,68,53,40,28,18,10,4,1,0,2,5,12,20,30,43,57,72,89,106,124,142,161,179,196,213,228,242,255,266,276,283,289,293,295,296,295,293,290,286,281,276,271,266,261,258,255,254,254,256,259,265,273,283,295,310,326,344,365,387,410,434,459,485,511,538,564,589,613,636,658,679,697,713,728,740,750,758,764,767,769,769,768,765,762,757,752,747,742,737,733,730,728,727,728,730,734,740,747,757,768,781,795,810,827,844,862,881,899,917,934,951,966,980,993,1003,1011,1018,1021,1023,1022,1019,1013,1005,995,983,970,955,939,921,903,885,867,849,831,814,799,784,771,759,750,741,735,731,728,727,727,729,732,736,741,746,751,756,761,765,768,769,770,768,765,760,752,743,731,717,701,683,664,642,619,595,570,544,518,492,466,440,416,392,370,349,331,314,299,286,275,267,261,256,254,253,254,257,260,265,269,275,280,285,289,292,295,296,296,294,290,285,278,269,258,246,232,217,200,183,165,147,129,111,93,76,60,46,33,22,13,7,2,0,0,3,8,16,25,36,50,64,80,97,115,133,152,170,188,205,221,235,249,261,271,280,286,291,294,296,296,294,292,288,283,278,273,268,263,259,256,254,253,254,257,262,269,278,289,302,318,335,354,375,398,422,447,472,498,525,551,576,601,625,648,669,688,705,721,734,745,754,761,766,769,770,769,767,764,760,755,750,745,740,735,731,729,727,727,729,732,737,743,752,762,774,788,802,818,835,853,871,890,908,926,943,959,973,987,998,1007,1015,1020,1023,1023,1021,1016,1010,1001,990,977,963,947,930,912,894,876,858,840,823,806,791,777,765,754,745,738,733,729,727,727,728,731,734,738,743,748,754,758,763,766,769,770,769,767,762,756,748,737,724,709,692,674,653,631,607,583,557,531,505,479,453,428,404,381,359,340,322,306,292,280,271,263,258,255,253,254,255,258,262,267,272,277,282,287,291,294,296,296,295,292,288,282,273,264,252,239,224,209,192,174,156,138,120,102,84,68,53,40,28,18,10,4,1,0,2,5,12,20,30,43,57,72,89,106,124,142,161,179,196,213,228,242,255,266,276,283,289,293,295,296,295,293,290,286,281,276,271,266,261,258,255,254,254,256,259,265,273,283,295,310,326,344,365,387,410,434,459,485,511,538,564,589,613,636,658,679,697,713,728,740,750,758,764,767,769,769,768,765,762,757,752,747,742,737,733,730,728,727,728,730,734,740,747,757,768,781,795,810,827,844,862,881,899,917,934,951,966,980,993,1003,1011,1018,1021,1023,1022,1019,1013,1005,995,983,970,955,939,921,903,885,867,849,831,814,799,784,771,759,750,741,735,731,728,727,727,729,732,736,741,746,751,756,761,765,768,769,770,768,765,760,752,743,731,717,701,683,664,642,619,595,570,544,518,492,466,440,416,392,370,349,331,314,299,286,275,267,261,256,254,253,254,257,260,265,269,275,280,285,289,292,295,296,296,294};
int16_t s2[]={253,270,286,302,318,333,347,360,372,382,391,399,405,409,413,414,415,414,413,410,407,404,400,397,394,391,389,388,389,390,393,398,404,411,421,432,445,459,474,491,509,528,547,567,587,607,627,646,665,682,699,714,727,739,750,758,765,770,773,775,774,773,770,766,761,756,750,744,738,732,726,721,717,715,713,712,713,715,719,724,730,738,747,757,767,779,791,803,815,827,839,850,860,869,877,883,888,890,891,891,888,883,877,869,859,847,835,821,806,790,774,758,741,725,709,694,680,666,654,643,634,626,619,615,611,609,608,609,610,612,615,618,622,625,628,631,633,634,635,633,631,627,621,614,605,594,582,568,553,536,519,500,481,461,441,421,401,381,363,345,328,313,299,286,276,267,260,254,251,249,248,250,252,256,260,266,271,278,284,290,295,300,305,308,310,311,310,309,305,301,295,287,279,269,258,247,235,223,211,199,187,176,165,156,148,141,136,133,132,132,134,138,144,152,162,173,185,199,213,229,245,261,278,294,310,325,340,354,366,377,387,395,402,407,411,414,415,415,414,412,409,406,402,399,395,392,390,389,388,389,391,395,400,407,416,426,438,451,466,483,500,518,537,557,577,597,617,637,656,674,691,707,721,734,745,754,762,768,772,774,775,774,772,768,764,759,753,747,741,735,729,724,719,716,713,712,712,714,717,721,727,734,742,751,762,773,785,797,809,821,833,845,855,865,873,880,885,889,891,891,890,886,880,873,864,853,841,828,813,798,782,766,749,733,717,701,687,673,660,649,638,630,623,617,613,610,608,608,609,611,613,617,620,623,627,630,632,634,635,634,632,629,624,618,609,600,588,575,561,545,528,509,490,471,451,431,411,391,372,354,336,320,306,292,281,271,263,257,252,249,248,249,251,254,258,263,268,274,281,287,293,298,303,306,309,311,311,310,307,303,298,291,283,274,264,253,241,229,217,205,193,181,170,161,152,145,139,135,132,132,133,136,141,148,157,167,179,192,206,221,237,253,270,286,302,318,333,347,360,372,382,391,399,405,409,413,414,415,414,413,410,407,404,400,397,394,391,389,388,389,390,393,398,404,411,421,432,445,459,474,491,509,528,547,567,587,607,627,646,665,682,699,714,727,739,750,758,765,770,773,775,774,773,770,766,761,756,750,744,738,732,726,721,717,715,713,712,713,715,719,724,730,738,747,757,767,779,791,803,815,827,839,850,860,869,877,883,888,890,891,891,888,883,877,869,859,847,835,821,806,790,774,758,741,725,709,694,680,666,654,643,634,626,619,615,611,609,608,609,610,612,615,618,622,625,628,631,633,634,635,633,631,627,621,614,605,594,582,568,553,536,519,500,481,461,441,421,401,381,363,345,328,313,299,286,276,267,260,254,251,249,248,250,252,256,260,266,271,278,284,290,295,300,305,308,310,311,310,309,305,301,295,287,279,269,258,247,235,223,211,199,187,176,165,156,148,141,136,133,132,132,134,138,144,152,162,173,185,199,213,229,245,261,278,294,310,325,340,354,366,377,387,395,402,407,411,414,415,415,414,412,409,406,402,399,395,392,390,389,388,389,391,395,400,407,416,426,438,451,466,483,500,518,537,557,577,597,617,637,656,674,691,707,721,734,745,754,762,768,772,774,775,774,772,768,764,759,753,747,741,735,729,724,719,716,713,712,712,714,717,721,727,734,742,751,762,773,785,797,809,821,833,845,855,865,873,880,885,889,891,891,890,886,880,873,864,853,841,828,813,798,782,766,749,733,717,701,687,673,660,649,638,630,623,617,613,610,608,608,609,611,613,617,620,623,627,630,632,634,635,634,632,629,624,618,609,600,588,575,561,545,528,509,490,471,451,431,411,391,372,354,336,320,306,292,281,271,263,257,252,249,248,249,251,254,258,263,268,274,281,287,293,298,303,306,309,311,311,310,307,303,298,291,283,274,264,253,241,229,217,205,193,181,170,161,152,145,139,135,132,132,133,136,141,148,157,167,179,192,206,221,237,253,270,286,302,318,333,347,360,372,382,391,399,405,409,413,414,415,414,413,410,407,404,400,397,394,391,389,388,389,390,393,398,404,411,421,432,445,459,474,491,509,528,547,567,587,607,627,646,665,682,699,714,727,739,750,758,765,770,773,775,774,773,770,766,761,756,750,744,738,732,726,721,717,715,713,712,713,715,719,724,730,738,747,757,767,779,791,803,815,827,839,850,860,869,877,883,888,890,891,891,888,883,877,869,859,847,835,821,806,790,774,758,741,725,709,694,680,666,654,643,634,626,619,615,611,609,608,609,610,612,615,618,622,625,628,631,633,634,635,633,631,627};

int32_t ifft_buf[FFTSIZE * 2];
arm_cfft_radix4_instance_q15 fft_inst;
arm_cfft_radix4_instance_q31 ifft_inst;


void setup() {
  Serial.begin(115200);
  arm_cfft_radix4_init_q15(&fft_inst, FFTSIZE, 0, 1);
  arm_cfft_radix4_init_q31(&ifft_inst, FFTSIZE, 1, 1);
  pinMode(ledPin, OUTPUT);
}

void loop() {

  //generating random signals
  /*
  // generate two signals with random data, s2 is delayed s1
  unsigned offset = random(FFTSIZE / 4);
  for (unsigned i = 0; i < FFTSIZE + offset; i++) {
    int16_t val = ((int16_t) random(65536));
    if (i < FFTSIZE)
      s1[i] = val;
    if (i > offset)
      s2[i - offset] = val;
  }
  */

  // set LED to high to show that data recording has begun
  digitalWrite(ledPin, HIGH);   // set the LED on

 /*
  // timing loop
  /////////////////////  /////////////////////  /////////////////////  /////////////////////
  long timeThen = millis();

  for (int j = 0; j < timingLoopSize; j++)  {
      s1[j] = analogRead(analogPin2);
      s2[j] = analogRead(analogPin4);
      delayMicroseconds(23);
    }  
    
  long timeNow = millis();
  
  Serial.print("time for "); Serial.print(timingLoopSize);
  Serial.print(" samples, in millis = "); Serial.println(timeNow - timeThen);  
*/  

/*
    // data collection loop, fill two microphone buffers with a delay to set an approximate sample rate
    for (int i = 0; i < FFTSIZE - 1; i++)  {
      s1[i] = analogRead(analogPin2);
      s2[i] = analogRead(analogPin4);
      delayMicroseconds(23);
    }

*/

  // set LED to LOW to show that data recording has ceased
  digitalWrite(ledPin, LOW);   // set the LED OFF

/*
  // print out two acquired buffers, make sure it looks right
  Serial.println("Microphone 2 = ");
  for (int i = 0; i < FFTSIZE; i++)  {
    Serial.print(s1[i]);  Serial.print("  ");
  }

  Serial.println("");

  Serial.println("Microphone 4 = ");
  for (int i = 0; i < FFTSIZE; i++)  {
    Serial.print(s2[i]);  Serial.print("  ");
  }

  Serial.println("");
*/

/*
  // Copy samples interleaved to FFT buffer s1 in real part, s2 in complex part
  // ie. fftbuf = s1 + j * s2
  for (int i = 0; i < FFTSIZE; i++)
    ((int32_t *)fft_buf)[i] = (s2[i] << 16) | s1[i];
 */


  elapsedMicros us; // start timing

  // Perform FFT
//  arm_cfft_radix4_q15(&fft_inst, fft_buf);


  // Taken from adafruit spectrum.ino code
  // Move s1 data into a samples buffer, with 0's for complex part every 2nd
  int j = 0;
  for (int i=0; i<2*FFTSIZE; i++)  {
      if ( (i & 0x01) == 0) { 
        samples[i] = 0; 
        }
      else {
        samples[i] = s1[j];
        j++;
      }
  }

  // Run FFT on sample data.
   arm_cfft_radix4_instance_q15 fft_instADA;
   arm_cfft_radix4_init_q15(&fft_instADA, FFTSIZE, 0, 1);
   arm_cfft_radix4_q15(&fft_instADA, samples);
  
  // Calculate magnitude of complex numbers output by the FFT.
  arm_cmplx_mag_q15(samples, magnitudes, FFTSIZE);

  for (int i=0; i < FFTSIZE; i++) {
    Serial.print(magnitudes[i]); Serial.print(" ");
  }
  Serial.println("");
  


  // Untangle fft of both signals and calculate conj(fft(s1)) * fft(s2)
  // Treat DC point special
  int16_t fs1_re, fs1_im, fs2_re, fs2_im;
  fs1_re = fft_buf[0]; // fs1_im = 0
  fs2_re = fft_buf[0]; // fs2_im = 0
  ifft_buf[0] = fs1_re * fs2_re;
  ifft_buf[1] = 0;
  // Then handle most other points
  int16_t *fft_fwd = fft_buf + 2;
  int16_t *fft_rev = fft_buf + 2 * (FFTSIZE - 1);
  for (size_t i = 2; i < FFTSIZE; i += 2, fft_fwd += 2, fft_rev -= 2) {
    fs1_re = (fft_fwd[0] + fft_rev[0]) >> 1;
    fs1_im = (fft_fwd[1] - fft_rev[1]) >> 1;
    fs2_re = (fft_rev[1] + fft_fwd[1]) >> 1;
    fs2_im = (fft_rev[0] - fft_fwd[0]) >> 1;
    // calculate conj(fft(s1)) * fft(s2)
    int32_t cfs1_fs2_re = (fs1_re * fs2_re + fs1_im * fs2_im);
    int32_t cfs1_fs2_im = (fs1_re * fs2_im - fs1_im * fs2_re);
    ifft_buf[i] = ifft_buf[2 * FFTSIZE - i] = cfs1_fs2_re;
    ifft_buf[i + 1] = cfs1_fs2_im;
    ifft_buf[2 * FFTSIZE - i + 1] = -cfs1_fs2_im;
  }
  // Finally handle Nyquist point
  fs1_re = fft_fwd[0]; // fs1_im = 0
  fs2_re = fft_fwd[1]; // fs2_im = 0
  ifft_buf[FFTSIZE] = fs1_re * fs2_re;
  ifft_buf[FFTSIZE + 1] = 0;

    //Print out FFT results for 1st signal
    for (int i = 0; i < FFTSIZE+1; i++)  {
    Serial.print(ifft_buf[i]);  Serial.print(" ");
  }
  Serial.println("");


  // Perform inverse FFT
  arm_cfft_radix4_q31(&ifft_inst, ifft_buf);

  uint32_t duration = us; // end of timing

  // The output data is in every second element of ifft_buf

  // As an example, find the cross-cor peak
  int peak_offset = 0;
  int32_t peak_value = ifft_buf[0];
  for (size_t i = 1, j = 2; i < FFTSIZE; i++, j += 2) {

    if (ifft_buf[j] > peak_value) {
      //      Serial.println(ifft_buf[j]);
      peak_value = ifft_buf[j];
      peak_offset = i;
    }
  }
  if (peak_offset > FFTSIZE / 2) {
    peak_offset -= FFTSIZE;
  }

  //  Serial.print("Generated offset:  ");
  //  Serial.println(offset);
  Serial.print("Calculated offset: ");
  Serial.println(-peak_offset); // delay implies negative
  //  Serial.print("It took ");
  //  Serial.print(duration);
  //  Serial.println(" us\n");

  Serial.println("");
  delay(5000);
}