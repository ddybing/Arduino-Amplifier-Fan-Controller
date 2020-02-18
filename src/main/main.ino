
/* -- PIN map --

  PIN A0 = Potentiometer
  PIN 2  = Switch
  PIN 3  = Fan 1 PWM
  PIN 5  = Fan 2 PWM
  PIN 6  = LED PWM
  PIN 7  = LED relay
  PIN 8  = Fan 1 relay
  PIN 12 = Signal from temp sensor (DS18B20)
  PIN 13 = Fan 2 relè

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
#define LED_CONTROL_PIN 6
#define LED_SWITCH_PIN 7
#define FAN1_SWITCH_PIN 8
#define TEMPSENSOR_PIN 12
#define FAN2_SWITCH_PIN 13

#define ONE_WIRE_BUS 12

//DeviceAddress tempSensor1[8] = {0x28, 0xAA, 0xB3, 0xB0, 0x16, 0x13, 0x02, 0x83};
//DeviceAddress tempSensor2[8] = {0x28, 0xAA, 0xA5, 0x84, 0x13, 0x13, 0x02, 0x10};



// Settings ||||||||||||||||||||||||||||||||||||||||||||||||||||


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

//fan1.switch_pin(FAN1_SWITCH_PIN);
//fan1.signal_pin(FAN1_CONTROL_PIN);
//fan1.state(0);
void setup(void)
{
  // Open serial port at 9600 baud.
  Serial.begin(9600);
  // -- Define and initialize I/O pins --


  // Physical switch input pin
  pinMode(SWITCH_PIN, INPUT);

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
    sensors.setResolution(i, 12);
  }

  Serial.print("### Arduino Receiver Cooling and Visualization System (A.R.C.V.S) ###");
  Serial.print("\n");

  digitalWrite(FAN1_SWITCH_PIN, LOW);
  digitalWrite(FAN2_SWITCH_PIN, LOW);


}


// END OF SETUP |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||




void setFanSpeed(float fanSpeed)
{
  analogWrite(FAN1_CONTROL_PIN, (fanSpeed * FAN1_SPEED_COMP));
  analogWrite(FAN2_CONTROL_PIN, (fanSpeed * FAN2_SPEED_COMP));
}


float getTemperature()
{
  sensors.requestTemperatures();
  delay(100);
  float temp = sensors.getTempCByIndex(0);
  return temp;
}


float calculateDutyCycle(float temp)
{
  // Return duty cycle for fan PWM (0-255).
  if (temp > MAX_TEMP) {
    return 255;
  }
  else {
    return 0.0625 * (temp * temp) + 0 + 30;
  }
}


void loop(void)
{

  float temp = getTemperature();
  float duty_cycle = calculateDutyCycle(temp);
  if (temp > MAX_TEMP)
  {
    setFanSpeed(255);
  }

  else if (temp < MIN_TEMP)
  {
    digitalWrite(LED_SWITCH_PIN, HIGH);  // led1.turnOff();
    digitalWrite(FAN1_SWITCH_PIN, HIGH); // fan1.turnOff();
    digitalWrite(FAN2_SWITCH_PIN, HIGH); // fan2.turnOff();
  }
  else
  {
    digitalWrite(LED_SWITCH_PIN, LOW);  // led1.turnOn();
    digitalWrite(FAN1_SWITCH_PIN, LOW); // fan1.turnOn();
    digitalWrite(FAN2_SWITCH_PIN, LOW); // fan2.turnOn();
    Serial.println(temp);
    Serial.println(duty_cycle);
    setFanSpeed(duty_cycle);
  }

  //Serial.print("Duty cycle: ");
  //Serial.println(duty_cycle);
  Serial.print("Bryter: ");
  Serial.println(digitalRead(SWITCH_PIN));
  delay(1000);

}
