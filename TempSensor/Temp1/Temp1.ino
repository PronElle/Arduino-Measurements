/********************************************************************/
// First we include the libraries
#include "OneWire.h" 
#include "DallasTemperature.h"
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 
void setup(void) 
{ 
 // start serial port 
 Serial.begin(57600); 
 Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
 sensors.begin(); 
} 
void loop(void) 
{ 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 Serial.print(" Requesting temperatures..."); 
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 Serial.println("DONE"); 
/********************************************************************/
 Serial.print("Temperature is: "); 
 Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
   delay(5); 

  xloop();
} 


void xloop() {
  // put your main code here, to run repeatedly:

  int X = analogRead(A2);
  float V = ((float) X) / 1024.0 * 5.0;

  Serial.println("X = " + String(X));

 // Serial.println("V = "+String(V,3)+" V");

  float T = V / 10e-3 - 273.15 + 3.0;
  
  Serial.println("analog T = "+String(T,2)+" °C");

  int Y = analogRead(A1);
  //Serial.println("dY = " + String(2.0/((float)Y)));
  Serial.println(Y);

  
  float V_LM = ((float) Y) / 1024.0 * 5.0;

  Serial.println("V_LM = "+String(V_LM,3)+" V");
  
  Serial.println("V_REF= " + String(4.1/((float)Y) * 1024.0)); 
   
  delay(500);

}
