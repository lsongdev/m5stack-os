
#include "time.h"
#include "HeaderDrawer.h"
#include "MenuCallBack.h"

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

class DateTimeSetting : public MenuCallBack {
public:
  bool setup() {
    Header.draw();
    M5.Lcd.fillScreen(0);
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 6);
    }
    M5.Lcd.drawString("DateTime", 10, 10, 2);
    btnDrawer.setText("Back/Prev","Ok","Next");
    btnDrawer.draw(true);

    //init and get the time
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = 3600;
    const int   daylightOffset_sec = 3600;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    return true;
  }

  bool loop() {
    Header.draw();
    printLocalTime();
    delay(1000);
    return true;
  }
};