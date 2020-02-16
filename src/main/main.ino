
 /* -- PIN map --
 * 
 * PIN A0 = Potentiometer
 * PIN 2  = Switch
 * PIN 3  = Fan 1 PWM
 * PIN 5  = Fan 2 PWM
 * PIN 7  = LED relay
 * PIN 8  = Fan 1 relay
 * PIN 12 = Signal from temp sensor (DS18B20)
 * PIN 13 = Fan 2 relè
 * 
 */

// Duty cycle formula for fans
// f(x) = ax^2
// 0.625 * pow (temp, 2) + 0 + 30 = duty_cycle
// E.g. temp = 30, duty_cycle = 86.25

#include <OneWire.h>
#include <math.h>
#include <FastLED.h>
#include <DallasTemperature.h>
/********************************************************************/

// Pin mapping
#define POTENTIOMETER_PIN A0
#define SWITCH_PIN 2
#define FAN1_CONTROL_PIN 3
#define FAN2_CONTROL_PIN 5
#define LED_SWITCH_PIN 7
#define FAN1_SWITCH_PIN 8
#define TEMPSENSOR_PIN 12
#define FAN2_SWITCH_PIN 13


// Temp sensor addresses
DeviceAddress tempSensor1 = {0x28, 0xAA, 0xB3, 0xB0, 0x16, 0x13, 0x02, 0x83}; // For vifte 1
DeviceAddress tempSensor2 = {0x28, 0xAA, 0xA5, 0x84, 0x13, 0x13, 0x02, 0x10}; // For vifte 2



// Classes ||||||||||||||||||||||||||||||||||||||||||||||||||||||

class Fan
{
  public:

        int switch_pin;
        int signal_pin;
        int state;
        
        void turnOff();

        void turnOn();
};

void Fan::turnOff()
{
  // Set fan speed to 10% duty cycle. 
  analogWrite (signal_pin, 25.5);
  delay (100); // Wait for 100 milliseconds
  digitalWrite (switch_pin, HIGH);
}


void Fan::turnOn()
{
  // Set fan speed to 10% duty cycle.
  analogWrite (signal_pin, 25.5);
  delay (100); // Wait for 100 milliseconds
  digitalWrite (switch_pin, LOW);
}
// End of Classes ||||||||||||||||||||||||||||||||||||||

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
 // Open serial port at 9600 baud. 
 Serial.begin(9600); 

 //DeviceAddress tempSensor1 = {0x28, 0xAA, 0xB3, 0xB0, 0x16, 0x13, 0x02, 0x83};
 //DeviceAddress tempSensor2 = {0x28, 0xAA, 0xA5, 0x84, 0x13, 0x13, 0x02, 0x10};
 // -- Define and initialize I/O pins -- 

 // Fan 1 relay state
 pinMode(FAN1_SWITCH_PIN, OUTPUT); 
 digitalWrite(FAN1_SWITCH_PIN, HIGH); // Default state

 // Fan 2 relay state
 pinMode(FAN2_SWITCH_PIN, OUTPUT); // 
 digitalWrite(FAN2_SWITCH_PIN, HIGH); // Default state


 // Initialize sensors
 sensors.begin();

 // Safety delay after sensors have initialized
 delay(2000);
 
 // Set sensor bit resolution to 12 for all sensors.
 int sensorCount = sensors.getDeviceCount();
 for (int i = 0; i < sensorCount; i++)
 {
  sensors.setResolution(i,12);
 }
 
 Serial.print("### Arduino Receiver Cooling and Visualization System (A.R.C.V.S) ###");
 Serial.print("\n");



} 


// END OF SETUP |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Fan fan1;


void setFanSpeed(float fanSpeed)
{
      analogWrite(FAN1_CONTROL_PIN, fanSpeed);
}


float getTemperature()
{
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  return temp;
}


float calculateDutyCycle(float temp)
{
  // Return duty cycle for fan PWM (0-255).
  return 0.625 * pow (temp, 2) + 0 + 30;
}


void loop(void) 
{ 
  float temp = getTemperature();
  float duty_cycle = calculateDutyCycle(temp);
  setFanSpeed(duty_cycle);
  
  

    
}
