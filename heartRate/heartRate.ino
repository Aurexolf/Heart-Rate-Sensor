#include "MAX30105.h"
#include "algorithm.h"

MAX30105 particleSensor;

const byte BUFFER_SIZE = 200;

int16_t irBuffer[BUFFER_LENGTH]; //infrared LED sensor data
int16_t redBuffer[BUFFER_LENGTH];  //red LED sensor data

int16_t spo2; //SPO2 value
boolean validSPO2; //indicator to show if the SPO2 calculation is valid
double heartRate; //heart rate value
byte validHeartRate; //indicator to show if the heart rate calculation is valid

//Initalise the MAX chip
void setup() {
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:
  
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    //Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  Serial.println(F("Attach sensor to finger with rubber band. Press any key to start conversion"));
  //while (Serial.available() == 0) ; //wait until user presses a key
  //Serial.read();

  byte ledBrightness = 20; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 69; //Options: 69, 118, 215, 411
  int adcRange = 16384; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
}


void loop() {
  byte count = 0;
  Serial.println(F("Loop Started"));
  //Read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < BUFFER_SIZE ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = (int16_t) particleSensor.getRed();
    irBuffer[i] = (int16_t) particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

    Serial.print(F("Red = "));
    Serial.print(redBuffer[i]);
    Serial.print(F(", IR = "));
    Serial.println(irBuffer[i]);
    Serial.println(++count);
  }

  Serial.println(F("Initial Values obtained"));

  
  calculate_heart(irBuffer, BUFFER_SIZE, &heartRate, &validHeartRate);

  //Continuously taking samples from MAX30102
  while (1)
  {
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < BUFFER_SIZE; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = BUFFER_SIZE - 25; i < BUFFER_SIZE; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      redBuffer[i] = (int16_t) particleSensor.getRed();
      irBuffer[i] = (int16_t) particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample
    }

    //After gathering 25 new samples recalculate HR and SP02
    calculate_heart(irBuffer, BUFFER_SIZE, &heartRate, &validHeartRate);

    if(validHeartRate){
      Serial.print(F("HR = "));
      Serial.println((int) heartRate, DEC);
    }
  }
  
}
