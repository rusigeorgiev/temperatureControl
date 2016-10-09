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
const int numReadingsSolar = 10; // this nuber shows how many readings will be averaged to calculate the temperature to smooth out noise
const int numReadingsBoiler = 10; // this nuber shows how many readings will be averaged to calculate the temperature to smooth out noise

float readingsSolar[numReadingsSolar]; // the readings from the analog input
int readIndexSolar = 0;              // the index of the current reading
float totalSolar = 0;                  // the running total
float averageSolar = 0;                // the average

float readingsBoiler[numReadingsBoiler];// the readings from the analog input
int readIndexBoiler = 0;              // the index of the current reading
float totalBoiler = 0;                  // the running total
float averageBoiler = 0;                // the average

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
  Serial.begin (9600); // start serial communication to PC
  
  pinMode(temperaturePinSolar, INPUT);
  pinMode(temperaturePinBoiler, INPUT);
  pinMode(relayPin, OUTPUT);

  for (int thisReading = 0; thisReading < numReadingsSolar; thisReading++) {
    readingsSolar[thisReading] = 0;
  }

  for (int thisReading = 0; thisReading < numReadingsBoiler; thisReading++) {
    readingsBoiler[thisReading] = 0;
  }

  Serial.print ("Setup ready");
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
  tmp = analogRead (temperaturePinSolar); // read from the sensor:
  // Serial.print ("Solar sensor = ");
  // Serial.println (tempSolar);
  tempSolar = calculateTemperature(tmp);
  
  totalSolar = totalSolar - readingsSolar[readIndexSolar]; // subtract the last reading:
  readingsSolar[readIndexSolar] = tempSolar;
  totalSolar = totalSolar + readingsSolar[readIndexSolar]; // add the reading to the total:
  readIndexSolar = readIndexSolar + 1; // advance to the next position in the array:

  // if we're at the end of the array...
  if (readIndexSolar >= numReadingsSolar) {
    readIndexSolar = 0; // ...wrap around to the beginning:
  }

  averageSolar = totalSolar / numReadingsSolar; // calculate the average:
  printTemperature("Solar average: ", averageSolar);

  // boiler temperature
  tmp = analogRead (temperaturePinBoiler); // read from the sensor:
  // Serial.print ("Boiler sensor = ");
  // Serial.println (tempBoiler);
  tempBoiler = calculateTemperature(tmp);
  
  totalBoiler = totalBoiler - readingsBoiler[readIndexBoiler]; // subtract the last reading:
  readingsBoiler[readIndexBoiler] = tempBoiler;
  totalBoiler = totalBoiler + readingsBoiler[readIndexBoiler]; // add the reading to the total:
  readIndexBoiler = readIndexBoiler + 1; // advance to the next position in the array:

  // if we're at the end of the array...
  if (readIndexBoiler >= numReadingsBoiler) {
    readIndexBoiler = 0; // ...wrap around to the beginning:
  }

  averageBoiler = totalBoiler / numReadingsBoiler; // calculate the average:
  printTemperature("Boiler average: ", averageBoiler);

  // calculate difference
  tempDifActual = abs(averageSolar - averageBoiler);
  Serial.print ("Difference: ");
  Serial.println (tempDifActual);

  // test if the relay must switch on
  if(tempDifActual > switchOnDif){
    digitalWrite(relayPin, HIGH);
    Serial.println ("Relay ON");
  } 

  // test if the relay must switch off
  if (tempDifActual < switchOffDif){
    digitalWrite(relayPin, LOW);
    Serial.println ("Relay OFF");
  }
  delay (1000);
}




















