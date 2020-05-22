#ifndef _SERIALPORT_VIEWER_H_
#define _SERIALPORT_VIEWER_H_

#include "HeaderDrawer.h"
#include "MenuCallBack.h"

HardwareSerial HWSerial(2);

class SerialPortViewer : public MenuCallBack
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
    M5.Lcd.drawString("SerialPort Viewer", 10, 10, 2);
    btnDrawer.setText("Back/Prev", "Ok", "Next");
    btnDrawer.draw(true);
    HWSerial.begin(9600);
    return true;
  }

  bool loop()
  {
    Header.draw();
    if (Serial.available())
    {
      int ch = Serial.read();
      HWSerial.write(ch);
    }

    if (HWSerial.available())
    {
      int ch = HWSerial.read();
      Serial.write(ch);
    }
    return true;
  }
};

#endif