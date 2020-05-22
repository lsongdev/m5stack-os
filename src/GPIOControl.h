#include <WiFi.h>
#include <HTTPClient.h>

#include "HeaderDrawer.h"
#include "MenuCallBack.h"

class GPIOControl : public MenuCallBack
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
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 6);
    }
    M5.Lcd.drawString("GPIO Control", 10, 10, 2);
    btnDrawer.setText("Back/Prev", "26", "36");
    btnDrawer.draw(true);

    pinMode(26, OUTPUT);
    pinMode(36, OUTPUT);
    return true;
  }

  bool loop() {
    delay(10);
    Header.draw();
    if(M5.BtnB.wasPressed()) {
      char b = digitalRead(26);
      digitalWrite(26, b == HIGH ? LOW : HIGH);
    }
    if(M5.BtnC.wasPressed()) {
      char c = digitalRead(36);
      digitalWrite(36, c == HIGH ? LOW : HIGH);
    }
    return true;
  }
};