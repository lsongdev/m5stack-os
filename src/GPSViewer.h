#ifndef _GPS_MODULE_H_
#define _GPS_MODULE_H_

#include <TinyGPS++.h>

#include "HeaderDrawer.h"
#include "MenuCallBack.h"

TinyGPSPlus gps;
HardwareSerial GPSModule(2);

class GPSViewer : public MenuCallBack
{
public:
  bool setup()
  {
    Header.draw();
    M5.Lcd.fillScreen(0);
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i)
    {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 1);
    }
    M5.Lcd.drawString("GPS Viewer", 10, 10, 2);
    btnDrawer.setText("Back/Prev", "Ok", "Next");
    btnDrawer.draw(true);
    GPSModule.begin(9600);
    return true;
  }

  bool loop()
  {
    Header.draw();
    if (Serial.available())
    {
      int ch = Serial.read();
      GPSModule.write(ch);
    }

    if (GPSModule.available())
    {
      int ch = GPSModule.read();
      gps.encode(ch);
    }

    if (gps.location.isValid())
    {
      M5.Lcd.setCursor(0, 70);
      M5.Lcd.print(F("LOCATION  Fix Age: "));
      M5.Lcd.println(gps.location.age());

      M5.Lcd.setCursor(0, 150);
      M5.Lcd.print(F("LAT: "));
      M5.Lcd.print(gps.location.lat(), 6);
      M5.Lcd.print(F(" - LNG: "));
      M5.Lcd.println(gps.location.lng(), 6);

      Serial.println(gps.location.lat());
      Serial.println(gps.location.lng());
    } else {
      Serial.print(".");
    }
    return true;
  }
};

#endif