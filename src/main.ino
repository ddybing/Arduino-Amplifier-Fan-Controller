
 /* -- PIN map --
 * 
 * PIN A0 = Potentiometer
 * PIN 2  = Bryter
 * PIN 3  = Vifte 1 PWM
 * PIN 5  = Vifte 2 PWM
 * PIN 7  = LED-lys relè
 * PIN 8  = Vifte 1 relè
 * PIN 12 = Temperatursensor signal
 * PIN 13 = Vifte 2 relè
 * 
 */



// Duty cycle formel 
// f(x) = ax^2
// 0.625 * pow (temp, 2) + 0 + 30 = duty_cycle
// E.g. temp = 30, duty_cycle = 86.25

#include <OneWire.h>
#include <math.h>
#include <FastLED.h>
#include <DallasTemperature.h>
/********************************************************************/

// Definisjon av pins,
#define POTENTIOMETER_PIN A0
#define SWITCH_PIN 2
#define FAN1_CONTROL_PIN 3
#define FAN2_CONTROL_PIN 5
#define LED_SWITCH_PIN 7
#define FAN1_SWITCH_PIN 8
#define TEMPSENSOR_PIN 12
#define FAN2_SWITCH_PIN 13


// Addresser til tilkoblede temperatursensorer
DeviceAddress tempSensor1 = {0x28, 0xAA, 0xB3, 0xB0, 0x16, 0x13, 0x02, 0x83}; // For vifte 1
DeviceAddress tempSensor2 = {0x28, 0xAA, 0xA5, 0x84, 0x13, 0x13, 0x02, 0x10}; // For vifte 2

/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(TEMPSENSOR_PIN); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 





// SETUP |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void setup(void) 
{ 
 // Åpne serial port med 9600 baud
 Serial.begin(9600); 

 //DeviceAddress tempSensor1 = {0x28, 0xAA, 0xB3, 0xB0, 0x16, 0x13, 0x02, 0x83};
 //DeviceAddress tempSensor2 = {0x28, 0xAA, 0xA5, 0x84, 0x13, 0x13, 0x02, 0x10};
 // -- Definer og initialiser output og input pins -- 

 // Vifte 1 relè
 pinMode(FAN1_SWITCH_PIN, OUTPUT); // Sett som output
 digitalWrite(FAN1_SWITCH_PIN, HIGH); // Default tilstand

 // Vifte 2 relè
 pinMode(FAN2_SWITCH_PIN, OUTPUT); // Sett som output
 digitalWrite(FAN2_SWITCH_PIN, HIGH); // Default tilstand


 // Start sensor bibliotek
 sensors.begin();

 // Safety delay
 delay(2000);
 
 // Sett sensor bit-resolution til 12 for alle temperatursensorer
 int sensorCount = sensors.getDeviceCount();
 for (int i = 0; i < sensorCount; i++)
 {
  sensors.setResolution(i,12); 
 }
 //sensors.setResolution(tempSensor1, 11);
 //sensors.setResolution(tempSensor2, 11);
 
 Serial.print("### Danil og Kristoffer's temperaturmåler ###");
 Serial.print("\n");
} 





void fanControl(int fanSpeed)
{
      //analogWrite(PD3, fanSpeed);
      analogWrite(FAN1_CONTROL_PIN, fanSpeed);

}


float getTemperature()
{
  sensors.requestTemperatures();
  //float temp1 = sensors.getTempCByIndex(tempSensor1);
  //float temp2 = sensors.getTempCByIndex(tempSensor2);
  //float temperatures[] = {temp1, temp2};
  float temperature = 25.1;
  return temperature;
}


float calculateDutyCycle(float temp)
{
  // Returner duty cycle for PWM-styringen til vifta (0-255).
  return 0.625 * pow (temp, 2) + 0 + 30;
}

void commands

void loop(void) 
{ 
  int speed = 0;
  float temp = getTemperature();
  Serial.print("\n");
  if (temp > 24) 
  {
    fanControl(255);  
  }

  else
  {
    fanControl(32);
  }
  
  delay(2000);   
}
