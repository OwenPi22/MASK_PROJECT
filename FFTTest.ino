#include "arduinoFFT.h"

#include "LedControl.h" //  need the library
LedControl lc=LedControl(12,11,10,2); // 

#define SAMPLES 64             //Must be a power of 2
#define SAMPLING_FREQUENCY 800 //Hz, must be less than 10000 due to ADC
#define PIN 7

int Analog_Pin = A0; 
int speaker = 9;
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];
 
void setup() {

      lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,15);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  lc.shutdown(1,false);// turn off power saving, enables display
  lc.setIntensity(1,15);// sets brightness (0~15 possible values)
  lc.clearDisplay(1);// clear screen
    
    Serial.begin(115200);
 
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}
 
void loop()
{   
    /*SAMPLING*/
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!
     
        vReal[i] = analogRead(0);
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
 
    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
 
    /*PRINT RESULTS*/
    //Serial.println(peak);     //Print out what frequency is the most dominant.
    
    int count = 0;
    for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
        if((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES >= 150 && (i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES < 350)
        {
          tone(speaker, (i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES);
          //Serial.println("IF");
          colorCol(count, (vReal[i]-20) / 50);
          count++;
          /*Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
          Serial.print(" ");
          Serial.println(vReal[i], 1);*/    //View only this line in serial plotter to visualize the bins
        }
       
    }
 
    //delay(100);  //Repeat the process every second OR:
    //while(1);       //Run code once
}


void colorCol(int col, int n)
{
  if(col < 8)
  {
    for(int i = 0; i < 8; i++)
    {
      if(i >= 4 - n && i <= 3 + n)
      {
        lc.setLed(1, i, col, true);
      }
      else
      {
        lc.setLed(1, i, col, false);
      }
    }
  }
  else
  {
    for(int i = 0; i < 8; i++)
    {
      if(i >= 4 - n && i <= 3 + n)
      {
        lc.setLed(0, i, col - 8, true);
      }
      else
      {
        lc.setLed(0, i, col - 8, false);
      }
    }
  }
}
