// Water Flow Sensor output processed to read in ml/min for ESP32(WEMOS LOLIN32)
#include <Wire.h>
#include "SSD1306Wire.h"
#include <OLEDDisplay.h>

//-----------------------------------------------------------------
//water flow sensor
//-----------------------------------------------------------------
volatile long diff;
volatile int hist_value[256];
volatile unsigned char hist_index;
volatile unsigned long hist_count;
volatile unsigned long hist_sum;
volatile unsigned long oldTime;
volatile unsigned long cloopTime;
volatile bool update_flag;
volatile bool update_lock;

const int flowsensor_pin = 15; // Sensor Input
void flow () // Interrupt function
{
  if (update_lock) return;

  cloopTime = micros();
  if ((oldTime < cloopTime) && (0 < oldTime))
  {
    diff = cloopTime - oldTime;
    hist_sum += diff - hist_value[hist_index];
    hist_value[hist_index++] = diff;
    update_flag = true;
    if (256 > hist_count) hist_count++;
  }
  oldTime = cloopTime;
}

static void init_port()
{
  pinMode(flowsensor_pin, INPUT);
  digitalWrite(flowsensor_pin, LOW); // Optional Internal Pull-Up
  attachInterrupt(flowsensor_pin, flow, RISING); // Setup Interrupt
  reset_hist();
}


//-----------------------------------------------------------------
//oled
//-----------------------------------------------------------------
SSD1306Wire display(0x3c, 5, 4);
static long ml_min;
static char msg[64];
static unsigned char wdt_count;

static void reset_hist()
{
  update_lock = true;
  delay(1);
  for (int i = 0; i < 256; i++) hist_value[i] = 0;
  hist_count = 0;
  hist_index = 0;
  hist_sum = 0;
  oldTime = 0;
  update_flag = false;
  update_lock = false;
}

static void init_oled()
{
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  wdt_count = 0;
}


//-----------------------------------------------------------------
// main
//-----------------------------------------------------------------
void setup()
{
  //water flow sensor
  init_port();
  init_oled();
  sei();
}

void loop ()
{
  if (update_flag)
  {
    //update
    update_flag = false;
    wdt_count = 4;
  }
  else if (0 < wdt_count)
  {
    //timeout
    wdt_count--;
    reset_hist();
  }

  //ml_min = (0 == hist_count) ? 0 : ((hist_count * 1000000) / (7.5 * hist_sum));  //7.5Q
  //Q=98
  //ml_min = (0 == hist_count) ? 0 : ((hist_count * 1000000 * 1000) / (98 * hist_sum));  //mL/Min
  ml_min = (0 == hist_count) ? 0 : ((hist_count * 10204082) / hist_sum);  //mL/Min
  display.clear();
  sprintf (msg, "%ld[ml/min]", ml_min);
  display.drawString(64, 24, msg);
  display.display();
  delay(500);
}
