// Water Flow Sensor output processed to read in litres/hour
#include <Wire.h>
#include "SSD1306Wire.h"
#include <OLEDDisplay.h>


//water flow sensor
volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 15; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
void flow () // Interrupt function
{
  flow_frequency++;
}

//oled
SSD1306Wire display(0x3c, 5, 4);

void setup()
{
  //water flow sensor
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, LOW); // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(flowsensor, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;

  //oled
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void loop ()
{
  currentTime = millis();
  // Every second, calculate and print litres/hour
  
  if (currentTime >= (cloopTime + 1000))
  {
    cloopTime = currentTime; // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    flow_frequency = 0; // Reset Counter
    //Serial.print(l_hour, DEC); // Print litres/hour
    //Serial.println(" L/hour");

    //output oled
    char msg[64];
    sprintf (msg, "%d L/hour", l_hour);

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_24);
    display.drawString(64, 24, msg);
    display.display();
  }
}
