/*
 * Sample example for Olimexino-328 Rev.C and thermistor 10 kOhm, B=3435
 * In order to work connect: 
 * one of the outputs of thermistor to 3.3V;
 * the other one to A0;
 * A0 through 10 kOhm to GND.
 * 
 *    ^ 5.5V                                relay  pin9 ______\______
 *    |                                                             |
 *   _|_                                                            |
 *  |NTC|                                                         __|__ GND
 *  |10K| - thermometers
 *  |___|
 *    |
 *    |
 *    +------------ A0 and A1
 *    |
 *   _|_
 *  |   |
 *  |10K|
 *  |___|
 *    |
 * ___|___  GND
 *   ___
*/

int switchOnDif = 7; // if there is tempDif degrees between the 2 thermometers the relay will be switched ON
int histeresis = 2; // it is used so that the relay does not switch on and off many times on the border temperature

int switchOffDif = switchOnDif - histeresis;

float tempDifActual = 0;// holds the actual temperature difference
boolean ON = 1;
boolean OFF = 0;

float tempSolar = 15;
float tempBoiler = 15;

int temperaturePinSolar = A0; // this is the pin connected to the solar cells
int temperaturePinBoiler = A1; // this is the pin connected to the boiler coil
int relayPin = 9; // this is the pin connected to the relay that turns on the pump

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
  // Serial.print ("Solar sensor = ");
  // Serial.println (tempSolar);
  tempSolar = calculateTemperature(tmp);

  printTemperature("Solar", tempSolar);

  // boiler temperature
  tmp = analogRead (temperaturePinBoiler);
  // Serial.print ("Boiler sensor = ");
  // Serial.println (tempBoiler);
  tempBoiler = calculateTemperature(tmp);

  printTemperature("Boiler", tempBoiler);

  tempDifActual = abs(tempSolar - tempBoiler);
  Serial.print ("Difference: ");
  Serial.println (tempDifActual);

  if(tempDifActual > switchOnDif){
    digitalWrite(relayPin, HIGH);
    Serial.println ("Relay ON");
  } 
  
  if (tempDifActual < switchOffDif){
    digitalWrite(relayPin, LOW);
    Serial.println ("Relay OFF");
  }
  delay (1000);
}




















