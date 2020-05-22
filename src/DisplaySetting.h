#include <Preferences.h>

#include "HeaderDrawer.h"
#include "MenuCallBack.h"

class DisplaySetting : public MenuCallBack {
public:
  bool setup() {
    Header.draw();
    M5.Lcd.fillScreen(0);
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 1);
    }
    M5.Lcd.drawString("Display", 10, 10, 2);
    btnDrawer.setText("Back/Prev","-","+");
    btnDrawer.draw(true);

    preferences.begin("display");
    _value = preferences.getUInt("brightness", 80);
    preferences.end();

    updateBrightness();

    return true;
  }

  bool loop() {
    delay(10);
    Header.draw();
    if(M5.BtnB.wasPressed() && _value > 0) {
      _value-=5;
      updateBrightness();
    }
    if(M5.BtnC.wasPressed() && _value < 100) {
      _value+=5;
      updateBrightness();
    }
    return true;
  }

protected:
  float _value = 50;
  Preferences preferences;
  void updateBrightness() {
    preferences.begin("display", false);
    preferences.putUInt("brightness", _value);
    preferences.end();

    M5.Lcd.fillRect(20, 60, 280, 20, 0);
    M5.Lcd.setBrightness(0xff * (_value / 100));
    M5.Lcd.progressBar(20, 60, 280, 20, (uint8_t)_value);
    M5.Lcd.setTextColor(0xffff, 0);
    M5.Lcd.drawString("Brightness (" + String((int)_value) + "%)  ", 20, 40, 2);
    delay(100);
  }
};