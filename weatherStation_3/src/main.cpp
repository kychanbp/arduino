#include <Arduino.h>
#include <Wire.h>
#include <SFE_BMP180.h>
#include "Adafruit_SHT31.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SH1106 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

Adafruit_SHT31 sht31 = Adafruit_SHT31();
SFE_BMP180 pressure;
  
#define ALTITUDE 48.0 

char temperature[4];
char humidity[4];
char pressure_c[4];

void setup() {
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC);
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  } 

  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
}
  
void loop() 
{
  char status;
  double T,P,p0;
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.print("T:");
  display.setCursor(10,20);
  display.print("H:");
  display.setCursor(10,40);
  display.print("P:");

  //temperature
  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(t);

    dtostrf(t, 3, 1, temperature);
    display.setCursor(30,0);
    display.print(temperature);

  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);

    dtostrf(h, 3, 1, humidity);
    display.setCursor(30,20);
    display.print(humidity);

  } else { 
    Serial.println("Failed to read humidity");
  }

  //pressure
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("P:");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.println("");

          dtostrf(p0, 4, 1, pressure_c);
          display.setCursor(30,40);
          display.print(pressure_c);
          display.display();
          delay(1000);

          }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}