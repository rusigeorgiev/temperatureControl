/*
 * Sample example for Olimexino-328 Rev.C and thermistor 10 kOhm, B=3435
 * In order to work connect: 
 * one of the outputs of thermistor to 3.3V;
 * the other one to A0;
 * A0 through 10 kOhm to GND.
 * 
 *    ^ 3.3V
 *    |
 *   _|_
 *  |NTC|
 *  |10K|
 *  |___|
 *    |
 *    |
 *    +------------ A0
 *    |
 *   _|_
 *  |   |
 *  |10K|
 *  |___|
 *    |
 * ___|___  GND
 *   ___
*/

int temperaturePinSolar = A0; // this is the pin connected to the solar cells
int temperaturePinBoiler = A1; // this is the pin connected to the boiler coil
int relayPin = 10; // this is the pin connected to the boiler coil

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode(temperaturePinSolar, INPUT);
  pinMode(temperaturePinBoiler, INPUT);
  pinMode(relayPin, OUTPUT);
  Serial.print ("Init ready");
}

float temperature;

#define  R0  ((float)10000)
#define B ((float)3435)
// R0 = 10000 [ohm]
// B  = 3435
// T0 = 25 [C] = 298.15, [K]
// r = (ADC_MAX * R0) / (ADC_VAL) - R0
// R_ = R0 * e ^ (-B / T0), [ohm] --> const ~= 0.09919 (10K);
// T = B/ln (r/R_), [K]

float calculateTemperature(int sensorValue){
  float r, temperature;

  r = ((1023.0*R0)/(float)sensorValue)-R0;
  temperature = B/log(r/0.09919) - 273.15;  // log is ln in this case
  
  return temperature;
}

void printTemperature(String location, float temperature){
  Serial.print (location);
  Serial.print (" temperature = ");
  Serial.print (temperature);
  Serial.println (", C");
}

void loop() {
  // put your main code here, to run repeatedly:
  int tmp;
  
  // solar temperature
  tmp = analogRead (temperaturePinSolar);
  Serial.print ("Solar sensor = ");
  Serial.println (tmp);

  printTemperature("Solar", calculateTemperature(tmp));

  // boiler temperature
  tmp = analogRead (temperaturePinBoiler);
  Serial.print ("Boiler sensor = ");
  Serial.println (tmp);

  printTemperature("Boiler", calculateTemperature(tmp));
  delay (1000);
}




















