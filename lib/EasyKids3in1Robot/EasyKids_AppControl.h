#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "soc/soc.h"           //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "driver/rtc_io.h"


BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

int ch1 = 1500;
int ch2 = 1500;
int ch3 = 1500;
int ch4 = 1500;

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

String receive_data = "";

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};


class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    receive_data = "";
    for (int i = 0; i < rxValue.length(); i++) {
      receive_data = receive_data + rxValue[i];
    }
    //Serial.println(receive_data);
    // decode V7RC protocol //
    if (receive_data.startsWith("SRT") && (receive_data.length() == 20)) {
      ch1 = (receive_data.substring(3, 7)).toInt();
      ch2 = (receive_data.substring(7, 11)).toInt();
      ch3 = (receive_data.substring(11, 15)).toInt();
      ch4 = (receive_data.substring(15, 19)).toInt();
      receive_data = "";

      // ch1  = !ch1 == 0 ? 1500 : ch1; 
      // ch2  = !ch2 == 0 ? 1500 : ch2; 
      // ch3  = !ch3 == 0 ? 1500 : ch3; 
      // ch4  = !ch4 == 0 ? 1500 : ch4; 
    }
  }
};

void AppControl_Begin() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //disable brownout detector
  Serial.begin(115200);
  // Create the BLE Device
  BLEDevice::init("");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  // Start the service
  // https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/BLEAdvertising.cpp
  pService->start();
  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEUUID uuid("6E400001");
  //Device Name
  String device_name = "ESP_CAR";
  oAdvertisementData.setName(device_name.c_str());
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void appControl() {

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(20);                      // give the bluetooth stack the chance to get things ready
    BLEDevice::startAdvertising();  // restart advertising
    //    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}