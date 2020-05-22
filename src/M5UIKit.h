#ifndef M5_UI_H
#define M5_UI_H

#include <M5Stack.h>

void UIText() {

}

void UIInput(){
  
}

const int TOAST_SHORT = 1e3;
const int TOAST_LONG = 3e3;

void UIToast(const String& str, int dur) {
  const int sw = 320;
  const int h = 30;
  const int w = M5.Lcd.textWidth(str) + 12;
  const int x = (320 / 2) - (w / 2);
  const int y = 100;
  const int padding = 3;
  M5.Lcd.fillRect(x, y, w, h, 0);
  M5.Lcd.drawRect(x + padding, y + padding, w - padding * 2, h - padding * 2, 0xFFFF);
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextColor(0xFFFF, 0);
  M5.Lcd.drawCentreString(str, 160, y, 2);
  delay(dur);
}


void UIProgressBar(int x, int y, int w, int h, int v) {
  M5.Lcd.fillRect(x, y, w, h, 0xeeee);
  M5.Lcd.fillRect(x, y, w * (v / 100), h, 0xf800);
}

#endif