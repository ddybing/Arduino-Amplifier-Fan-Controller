
/* -- PIN map --

  PIN A0 = Potentiometer
  PIN 2  = Switch
  PIN 3  = Fan 1 PWM
  PIN 5  = Fan 2 PWM
  PIN 6  = LED PWM
  PIN 7  = LED relay
  PIN 8  = Fan 1 relay
  PIN 12 = Signal from temp sensor (DS18B20)
  PIN 13 = Fan 2 relay

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
#define INDICATOR_SWITCH_PIN 4
#define SWITCH_PIN 2
#define FAN1_CONTROL_PIN 3
#define FAN2_CONTROL_PIN 5
#define LED_CONTROL_PIN 6
#define LED_SWITCH_PIN 7
#define FAN1_SWITCH_PIN 8
#define TEMPSENSOR_PIN 12
#define FAN2_SWITCH_PIN 13

#define ONE_WIRE_BUS 12

//DeviceAddress tempSensor1[8] = {0x28, 0xAA, 0xB3, 0xB0, 0x16, 0x13, 0x02, 0x83};
//DeviceAddress tempSensor2[8] = {0x28, 0xAA, 0xA5, 0x84, 0x13, 0x13, 0x02, 0x10};


// Settings ||||||||||||||||||||||||||||||||||||||||||||||||||||

#define LED_BRIGHTNESS 10 // LED brightness 0-255
#define NUM_LEDS 30

#define OUT_OF_RANGE_FLASHES 20

#define MIN_TEMP 30
#define MAX_TEMP 60

#define FAN1_SPEED_COMP 1 // Speed compensation for fan 1. 1 = no compensation (duty_cycle x 1)
#define FAN2_SPEED_COMP 1 // Speed compensation for fan 2. 1 = no compensation (duty_cycle x 1)


// End of Settings |||||||||||||||||||||||||||||||||||||||||||||

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
  delay (100); // Wait for 100 milliseconds.
  digitalWrite (switch_pin, HIGH);
  delay(100); // Wait for relay to turn off.
}


void Fan::turnOn()
{
  // Set fan speed to 10% duty cycle.
  analogWrite (signal_pin, 25.5);
  delay (100); // Wait for 100 milliseconds.
  digitalWrite (switch_pin, LOW);
  delay(100); // Wait for relay to turn on.
}



class LED
{
  public:

    int switch_pin;
    int signal_pin;
    int state;

    void turnOff();

    void turnOn();

    void changeColour();

};


void LED::turnOff()
{
  // Set color to black
  // ....
  delay(100); // Wait 100 milliseconds
  digitalWrite(switch_pin, HIGH); // Turn off relay.
  delay(100); // Wait for relay to turn off.
}

void LED::turnOn()
{
  // Set color to black
  // ....
  delay(100); // Wait 100 milliseconds
  digitalWrite(switch_pin, HIGH);
  delay(100); // Wait for the relay to turn on.

}

void LED::changeColour()
{

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

  // Initialize LED
  CRGB leds[NUM_LEDS];
  


void setup(void)
{

  // Initialize LEDS and set brightness
  FastLED.addLeds<WS2812B, LED_CONTROL_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);

  
  // Open serial port at 9600 baud.
  Serial.begin(9600);
  // -- Define and initialize I/O pins --


  // Physical switch input pin. Activate pull-up resistor. NOTE: THIS INVERTS THE SIGNAL, Switch ON = 0, Switch OFF = 1.
  pinMode(SWITCH_PIN, INPUT);
  digitalWrite(SWITCH_PIN, HIGH);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // Indicator switch pin
  pinMode(INDICATOR_SWITCH_PIN, OUTPUT);

  // LED switch pin
  pinMode(LED_SWITCH_PIN, OUTPUT);

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

  Serial.print("### Arduino Receiver Cooling and Visualization System (A.R.C.V.S) ###");
  Serial.print("\n");
}


// END OF SETUP |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void setFanSpeed(float fanSpeed)
{
  analogWrite(FAN1_CONTROL_PIN, (fanSpeed * FAN1_SPEED_COMP));
  analogWrite(FAN2_CONTROL_PIN, (fanSpeed * FAN2_SPEED_COMP));
}


void setLEDTemperature(int temp)
{
  int modifier;
  if (temp < MIN_TEMP)
  {
    modifier = 30;
  }
  else if ( temp >= MAX_TEMP)
  {
    modifier = 60;
  }
  else
  {
    modifier = temp;
  }
  int green = 255-(modifier-30)*8.50;
  int blue = 0;
  int red = (modifier-30)*8.50;
  
  for (int i=0; i<NUM_LEDS; i++)
  {
    leds[i].red = red;
    leds[i].green = green;
    leds[i].blue = blue;
    FastLED.show();
    delay(50);
  }
}

float getTemperature()
{
  sensors.requestTemperatures();
  float temp1 = sensors.getTempCByIndex(0);
  float temp2 = sensors.getTempCByIndex(1);
  float temp;

  if (1 < temp1 | 1 < temp2) // Checking that the temp is an actual value and not a negative number (temp sensor error). 
  {
    temp = (temp1+temp2)/2;
    return temp;
  }

  else if (1 > temp1 & 1 < temp2)
  {
    return temp2;
  }

  else if (1 < temp1 & 1 > temp2)
  {
    return temp1;
  }
  else 
  {
    return (temp1+temp2)/2;
  }
}


float calculateManualDutyCycle(float temp)
{
  // Return duty cycle for fan PWM (0-255).
  if (temp > MAX_TEMP) {
    return 255;
  }
  else {
    float duty_cycle =  0.0625 * (temp * temp) + 0 + 30 + (analogRead(POTENTIOMETER_PIN)/5);
    if (duty_cycle >= 255)
    {
      return 255;
    }

    else{
      return 0.0625 * (temp * temp) + 0 + 30 + (analogRead(POTENTIOMETER_PIN)/5);
    }
  }
}

float failSafe(int temp) 
{
  // Check for issues

  if (temp < 15 | temp > 62)
  {
    for (int i=0; i<OUT_OF_RANGE_FLASHES; i++)
    {
      digitalWrite(INDICATOR_SWITCH_PIN, LOW);
      delay(160),
      digitalWrite(INDICATOR_SWITCH_PIN, HIGH);
    }
  }
    else
    {
      digitalWrite(INDICATOR_SWITCH_PIN, HIGH);
    }
}

float automaticControl(float temp)
{
  float duty_cycle = calculateDutyCycle(temp);
  if (temp >= MAX_TEMP)
  {
    setLEDTemperature(MAX_TEMP);
    setFanSpeed(255);
  }
  else if  (temp < 0)
  {
    setLEDTemperature(MAX_TEMP);
    setFanSpeed(255);
   }
  else if (temp < (MIN_TEMP-2)) // If temperature lower than 3 degrees below minimum temp
  {
    digitalWrite(LED_SWITCH_PIN, HIGH);  // led1.turnOff();
    digitalWrite(FAN1_SWITCH_PIN, HIGH); // fan1.turnOff();
    digitalWrite(FAN2_SWITCH_PIN, HIGH); // fan2.turnOff();
   }
    else if (temp >= MIN_TEMP)
   {
    digitalWrite(LED_SWITCH_PIN, LOW);  // led1.turnOn();
    digitalWrite(FAN1_SWITCH_PIN, LOW); // fan1.turnOn();
    digitalWrite(FAN2_SWITCH_PIN, LOW); // fan2.turnOn();
    setFanSpeed(duty_cycle);
    setLEDTemperature(temp);
    }   
}

float calculateDutyCycle(float temp)
{
  // Return duty cycle for fan PWM (0-255).
  if (temp > MAX_TEMP) 
  {
    return 255;
  }
  else {
    return 0.0625 * (temp * temp) + 0 + 30;
  }
}

int isAuto = 0;

void loop(void)
{
  int isAuto = digitalRead(SWITCH_PIN);

  while (isAuto == 1)
  {
    float temp = getTemperature();
    Serial.print(temp);
    automaticControl(temp);
    failSafe(temp); // Failsafe to run for each cycle
    isAuto = digitalRead(SWITCH_PIN);
    
    delay(500); // Delay between each cycle
   }  
    
    
  while (isAuto == 0)
  {
    float temp = getTemperature();
    Serial.print(temp);
    failSafe(temp); // Failsafe to run for each cycle
    float duty_cycle = (analogRead(POTENTIOMETER_PIN)/4);
    float fanDutyCycle = calculateManualDutyCycle(temp);
    digitalWrite(LED_SWITCH_PIN, LOW);  // led1.turnOn();
    digitalWrite(FAN1_SWITCH_PIN, LOW); // fan1.turnOn();
    digitalWrite(FAN2_SWITCH_PIN, LOW); // fan2.turnOn();
    setFanSpeed(fanDutyCycle);
    setLEDTemperature(temp);
    isAuto = digitalRead(SWITCH_PIN);
 
    delay(500); // Delay between each cycle
  }  
}
