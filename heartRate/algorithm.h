#include "Arduino.h"
#include <Math.h>

const uint16_t BUFFER_SIZE = 75;
const float lowHeart = 40.0/60.0;
const float highHeart = 200.0/60.0;
const int16_t sampleFreq = 25;

void calculate_heart(int16_t *irBuffer, double *heartRate, boolean *validHeartRate);

void calculate_SPO2(int16_t *irBuffer, int16_t *redBuffer, int16_t spo2, boolean validSPO2);

float checkCorrellation(int16_t *irBuffer, int16_t *redBuffer);

float rf_rms(float *pn_x, int32_t n_size, float *sumsq);

float rf_Pcorrelation(float *pn_x, float *pn_y, int32_t n_size);
