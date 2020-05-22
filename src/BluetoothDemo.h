#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "HeaderDrawer.h"
#include "MenuCallBack.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

M5OnScreenKeyboard m5osk;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    M5.Lcd.println("connect");
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    M5.Lcd.println("disconnect");
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    M5.Lcd.println("read");
  }

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    M5.Lcd.setTextFont(2);
    M5.Lcd.println(value.c_str());
  }
};

class BluetoothDemo : public MenuCallBack
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
    M5.Lcd.drawString("Bluetooth Demo", 10, 10, 2);
    btnDrawer.setText("Back/Prev", "Ok", "Next");
    btnDrawer.draw(true);
    m5osk.useFACES = true;

    BLEDevice::init("m5-stack");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY |
      BLECharacteristic::PROPERTY_INDICATE);
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.setTextFont(2);
    M5.Lcd.println("Starting Bluetooth Server...");
    m5osk.setup();
    return true;
  }

  bool loop()
  {
    delay(10);
    Header.draw();
    if (deviceConnected)
    {
      while(m5osk.loop()){
        delay(1);
      }
      String text = m5osk.getString();
      pCharacteristic->setValue(text.c_str());
      pCharacteristic->notify();
      m5osk.close();
    }
    return true;
  }
    
};