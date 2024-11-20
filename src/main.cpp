#define M5STACK_MPU6886
#include <vector>
#include <M5Stack.h>
#include <M5TreeView.h>
#include <M5OnScreenKeyboard.h>
#include <MenuItemSD.h>
#include <MenuItemSPIFFS.h>
#include <MenuItemToggle.h>
#include <MenuItemWiFiClient.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <Adafruit_NeoPixel.h>

#include "./HeaderDrawer.h"
#include "./SystemInfo.h"
#include "./SerialPortViewer.h"
#include "./GPSViewer.h"
#include "./BluetoothDemo.h"
#include "./MPU9250Demo.h"
#include "./I2CScanner.h"
#include "./BinaryViewer.h"
#include "./GPIOControl.h"
#include "./DisplaySetting.h"
#include "./DateTimeSetting.h"
#include "./M5UIKit.h"

M5TreeView treeView;

#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);

void callBackWiFiClient(MenuItem *sender) {
  MenuItemWiFiClient *mi = static_cast<MenuItemWiFiClient *>(sender);
  if (!mi) return;

  if (mi->ssid == "") return;

  Preferences preferences;
  preferences.begin("wifi-config");
  preferences.putString("WIFI_SSID", mi->ssid);
  String wifi_passwd = preferences.getString(mi->ssid.c_str());
  if (wifi_passwd == "")
    wifi_passwd = preferences.getString("WIFI_PASSWD");

  WiFi.disconnect();
  WiFi.mode(WIFI_MODE_STA);
  if (mi->auth != WIFI_AUTH_OPEN) {
    M5OnScreenKeyboard osk;
    osk.useFACES = true;
    osk.useCardKB = true;
    osk.useJoyStick = true;
    osk.usePLUSEncoder = true;
    osk.useFACESEncoder = true;
    osk.setup(wifi_passwd);
    while (osk.loop()) {
      delay(1);
    }
    wifi_passwd = osk.getString();
    osk.close();
    WiFi.begin(mi->ssid.c_str(), wifi_passwd.c_str());
    preferences.putString("WIFI_PASSWD", wifi_passwd);
    preferences.putString(mi->ssid.c_str(), wifi_passwd);
  } else {
    WiFi.begin(mi->ssid.c_str(), "");
    preferences.putString("WIFI_PASSWD", "");
  }
  preferences.end();
}

void callBackWiFiOff(MenuItem *sender) {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(true);
}

void callBackFormatSPIFFS(MenuItem *sender) {
  UIToast("SPIFFS Format...", TOAST_LONG);
  SPIFFS.begin();
  SPIFFS.format();
  SPIFFS.end();
}

void callBackFormatNVS(MenuItem *sender) {
  UIToast("NVS erase...", TOAST_LONG);
  nvs_flash_init();
  nvs_flash_erase();
  nvs_flash_deinit();
  nvs_flash_init();
  delay(1000);
}

void callBackAbout(MenuItem *sender) {
  UIToast("hello world", TOAST_LONG);
}

void callBackDeepSleep(MenuItem *sender) {
  M5.Lcd.setBrightness(0);
  M5.Lcd.sleep();
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_B_PIN, LOW);
  esp_deep_sleep_start();
}

void callBackRestart(MenuItem *sender) {
  ESP.restart();
}

template <class T>
void callBackExec(MenuItem *sender) {
  T menucallback;
  menucallback(sender);
}

typedef std::vector<MenuItem *> vmi;

std::vector<MenuItem *> initMenu() {
  return vmi {
    new MenuItem("WiFi", vmi {
      new MenuItemWiFiClient("WiFi Client", callBackWiFiClient),
      new MenuItem("WiFi Setting", 0),
      new MenuItem("WiFi Off", callBackWiFiOff)
    }),
    new MenuItem("Tools", vmi{
      new MenuItem("I2C Scanner", callBackExec<I2CScanner>),
      new MenuItem("IMU Sensor", callBackExec<MPU9250Demo>),
      new MenuItem("GPIO Control", callBackExec<GPIOControl>),
      new MenuItem("Bluetooth", callBackExec<BluetoothDemo>),
      new MenuItem("SerialPort", callBackExec<SerialPortViewer>),
      // new MenuItem("GPS Viewer", callBackExec<GPSViewer>),
      new MenuItem("Binary Viewer", vmi{
        new MenuItemSD("SDCard", callBackExec<BinaryViewerFS>),
        new MenuItemSPIFFS("SPIFFS", callBackExec<BinaryViewerFS>),
        new MenuItem("Flash", vmi{
          new MenuItem("2nd boot loader", 0, callBackExec<BinaryViewerFlash>),
          new MenuItem("partition table", 1, callBackExec<BinaryViewerFlash>),
          new MenuItem("nvs", 0x102, callBackExec<BinaryViewerFlash>),
          new MenuItem("otadata", 0x100, callBackExec<BinaryViewerFlash>),
          new MenuItem("app0", 0x010, callBackExec<BinaryViewerFlash>),
          new MenuItem("app1", 0x011, callBackExec<BinaryViewerFlash>),
          new MenuItem("eeprom", 0x199, callBackExec<BinaryViewerFlash>),
          new MenuItem("spiffs", 0x182, callBackExec<BinaryViewerFlash>)
        })
      })
    }),
    new MenuItem("Settings", vmi{
      new MenuItem("Display", callBackExec<DisplaySetting>),
      new MenuItem("DateTime", callBackExec<DateTimeSetting>),
      new MenuItem("Power", vmi{
        new MenuItem("Sleep", callBackDeepSleep),
        new MenuItem("Restart", callBackRestart),
        new MenuItem("Power Off", callBackDeepSleep)
      }),
      new MenuItem("Firmware Update", 0),
      new MenuItem("Factory Reset", callBackFormatNVS)
    }),
    new MenuItem("System Info", callBackExec<SystemInfo>),
    new MenuItem("About", callBackAbout)
  };
};

void drawFrame() {
  Rect16 r = treeView.clientRect;
  r.inflate(1);
}

void setup() {

  M5.begin();
  M5.Speaker.begin();
  M5.Speaker.mute();

  Wire.begin();
  pixels.begin();

  treeView.clientRect.x = 2;
  treeView.clientRect.y = 16;
  treeView.clientRect.w = 320;
  treeView.clientRect.h = 200;
  treeView.itemWidth = 320;

  treeView.useFACES = true;
  treeView.useCardKB = true;
  treeView.useJoyStick = true;
  treeView.usePLUSEncoder = true;
  treeView.useFACESEncoder = true;

  drawFrame();

  treeView.setItems(initMenu());
  treeView.begin();

  Preferences preferences;

  preferences.begin("display");
  const uint8_t brightness = preferences.getUInt("brightness", 80);
  // screen brightness
//  M5.Lcd.setBrightness(0xff * ((float)brightness / 100));
  preferences.end();

  preferences.begin("wifi-config");
  String wifi_ssid = preferences.getString("WIFI_SSID");
  String wifi_pass = preferences.getString("WIFI_PASSWD");
  preferences.end();
  // wifi
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());

}

uint8_t loopcnt = 0xF;

void loop() {
  MenuItem* mi = treeView.update();
  if (mi != NULL) {
    Serial.println("menu:" + mi->title + " / tag:" + mi->tag);
  }
  if (treeView.isRedraw()) {
    drawFrame();
    loopcnt = 0xF;
  }
  if (0 == (++loopcnt & 0xF)) {
    Header.draw();

    static int pixelNumber = 0; // = random(0, M5STACK_FIRE_NEO_NUM_LEDS - 1);
    pixelNumber++;
    if (pixelNumber > 9)pixelNumber = 0;
    int r = 1 << random(0, 7);
    int g = 1 << random(0, 7);
    int b = 1 << random(0, 7);

    pixels.setPixelColor(pixelNumber, pixels.Color(r, g, b));
    pixels.show();

  }

}
