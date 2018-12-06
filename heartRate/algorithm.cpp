#include "algorithm.h"

float irChangeBuffer[BUFFER_SIZE];
float redChangeBuffer[BUFFER_SIZE];

void calculate_heart(int16_t *irBuffer, double *heartRate, boolean *validHeartRate){
  //Variables
  int32_t sum = 0, highSum = 0;
  int16_t avg, period = 0;
  int16_t thresh = 0;
  uint16_t i;
  int8_t k;
  
  //Calculate DC average
  for(i = 0; i < BUFFER_SIZE; i++){
    avg += irBuffer[i];
  }

  avg /= BUFFER_SIZE;

  for(i = 0; i < BUFFER_SIZE; i++){
    irChangeBuffer[i] = irBuffer[i] - avg;
    thresh = max(thresh, irChangeBuffer[i]);
  }

  thresh /= 2;
  
  // sampleFreq/highHeart = smallest period we want to measure and sampleFreq/lowHeart = the highest
  for (int i = (float(sampleFreq)/highHeart); i < (int) (float(sampleFreq)/lowHeart); i++)
  {
    sum = 0;
    

        for (k=0; k <BUFFER_SIZE-i; k++)
   {
     sum += (irChangeBuffer[k]) * (irChangeBuffer[k+i]);
   }
   sum /= k ;
    
   if(sum > thresh && sum > highSum){
    period = i;
    highSum = sum;
   }
    
  }
  *validHeartRate = period ? 1:0;
  *heartRate = (double) 60*sampleFreq/period;
   
  return;
}

void calculate_SPO2(int16_t *irBuffer, int16_t *redBuffer, int16_t spo2, boolean validSPO2){
  int16_t avgIr, avgRed;
  float rmsIr, rmsRed, sqIr, sqRed, correl;
  
  ratio= (rmsIr*avgIr)/(rmsRed*avgRed);  
  if(ratio>0.02 && ratio<1.84) { 
    *spo2 = (-45.060*ratio + 30.354)*ratio + 94.845;
    *validSPO2 = 1;
  } else {
    *validSPO2  = 0; 
  }
}

float checkCorrellation(int16_t *irBuffer, int16_t *redBuffer){
  uint16_t i;
  int16_t avgIr, avgRed;
  float rmsIr, rmsRed, sqIr, sqRed, correl;
  
  
  //Calculate DC average
  for(i = 0; i < BUFFER_SIZE; i++){
    avgIr += irBuffer[i];
    avgRed += redBuffer[i];
  }

  avgIr /= BUFFER_SIZE;
  avgRed /= BUFFER_SIZE;

  

  for(i = 0; i < BUFFER_SIZE; i++){
    irChangeBuffer[i] = irBuffer[i] - avgIr;
    redChangeBuffer[i] = redBuffer[i] - avgRed;
  }

  rmsIr = rf_rms(irChangeBuffer, BUFFER_SIZE, &sqIr);
  rmsRed = rf_rms(redChangeBuffer, BUFFER_SIZE, &sqRed);
  
  correl=rf_Pcorrelation(irChangeBuffer, redChangeBuffer, BUFFER_SIZE)/sqrt(sqIr*sqRed);
  return correl;
}

float rf_rms(float *pn_x, int32_t n_size, float *sumsq) 
/**
* \brief        Root-mean-square variation 
* \par          Details
*               Compute root-mean-square variation for a given series pn_x
*               Robert Fraczkiewicz, 12/25/2017
* \retval       RMS value and raw sum of squares
*/
{
  int16_t i;
  float r,*pn_ptr;
  (*sumsq)=0.0;
  for (i=0,pn_ptr=pn_x; i<n_size; ++i,++pn_ptr) {
    r=(*pn_ptr);
    (*sumsq) += r*r;
  }
  (*sumsq)/=n_size; // This corresponds to autocorrelation at lag=0
  return sqrt(*sumsq);
}

float rf_Pcorrelation(float *pn_x, float *pn_y, int32_t n_size)
/**
* \brief        Correlation product
* \par          Details
*               Compute scalar product between *pn_x and *pn_y vectors
*               Robert Fraczkiewicz, 12/25/2017
* \retval       Correlation product
*/
{
  int16_t i;
  float r,*x_ptr,*y_ptr;
  r=0.0;
  for (i=0,x_ptr=pn_x,y_ptr=pn_y; i<n_size; ++i,++x_ptr,++y_ptr) {
    r+=(*x_ptr)*(*y_ptr);
  }
  r/=n_size;
  return r;
}
