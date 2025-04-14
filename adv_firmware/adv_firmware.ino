#include <FastLED.h>

#define LED_PIN     6     // Пин, к которому подключена лента
#define NUM_LEDS    3    // Количество светодиодов в ленте
#define BRIGHTNESS  255   // Яркость (0-255)
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB   // Порядок цветов (может отличаться для разных лент)

CRGB leds[NUM_LEDS];

#define PHSensorPin  A2    //dissolved oxygen sensor analog output pin to arduino mainboard
#define VREF 5.0    //for arduino uno, the ADC reference is the AVCC, that is 5.0V(TYP)
#define OFFSET -0.43  //zero drift compensation

#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    //store the analog value in the array, readed from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;

float averageVoltage,phValue;
String phstr;
char z;
static uint8_t hue = 0;
static uint8_t hues = 0;

void setup()
{
  pinMode(PHSensorPin,INPUT);
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
   fill_rainbow(leds, NUM_LEDS, hue, 7);  // 7 - шаг изменения оттенка
   FastLED.show();
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 30U)     //every 30 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(PHSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT)
         analogBufferIndex = 0;
   }
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 1000U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
      {
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      }
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the value more stable by the median filtering algorithm
      phValue = 3.5 * averageVoltage + OFFSET;
      phstr = String(phValue);
      Serial.print(phstr);
      delay(100);
   }
   if (Serial.available() > 0) {
     z = char(Serial.read());
     if (z == 'A'){
       digitalWrite(2, LOW);
       digitalWrite(4, LOW);
     }
     if (z == 'B'){
       digitalWrite(2, HIGH);
     }
     if (z == 'C'){
       digitalWrite(4, HIGH);
    }
   }
   hues++;
   if (hues > 5) {
    hues = 0;
    hue++;
   }
   if (hue > 255) {
    hue = 0;
   }
}

int getMedianNum(int bArray[], int iFilterLen)
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      {
      bTab[i] = bArray[i];
      }
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++)
      {
      for (i = 0; i < iFilterLen - j - 1; i++)
          {
        if (bTab[i] > bTab[i + 1])
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}
