#include "Arduino.h"
#include <Math.h>

const byte lowHeart = 40;
const int highHeart = 200;
const int16_t sampleFreq = 1800;

void calculate_heart(int16_t *irBuffer, const byte buffLength, double *heartRate, byte *validHeartRate);

double autocorr(int16_t *sample, int8_t len, float sampleFreq);


void calculate_heart(int16_t *irBuffer, const byte buffLength, double *heartRate, byte *validHeartRate){
  //Variables
  int32_t sum = 0, highSum = 0;
  int16_t colourBuffer[buffLength];
  int16_t avg, period = 0;
  int16_t thresh = 0;
  int8_t i, k;
  
  //Calculate DC average
  for(i = 0; i < buffLength; i++){
    avg += irBuffer[i];
  }

  avg /= buffLength;

  for(i = 0; i < buffLength; i++){
    colourBuffer[i] = irBuffer[i] - avg;
    thresh = max(thresh, colourBuffer[i]);
  }

  thresh /= 2;
  
  // sampleFreq/highHeart = smallest period we want to measure and sampleFreq/lowHeart = the highest
  for (int i = sampleFreq/highHeart; i < (int) sampleFreq/lowHeart; i++)
  {
    sum = 0;
    

        for (k=0; k <buffLength-i; k++)
   {
     sum += (colourBuffer[k]) * (colourBuffer[k+i]);
   }
   sum /= k ;
    
   if(sum > thresh && sum > highSum){
    period = i;
    highSum = sum;
   }
    
  }
  *validHeartRate = period ? 1:0;
  *heartRate = (double) sampleFreq/period;
   
  return;
}
