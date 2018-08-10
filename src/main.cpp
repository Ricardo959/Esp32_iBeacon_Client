/**
 * BLE client to scan and identify iBeacons.
 * Based on Neil Kolban's examples on https://github.com/nkolban/ESP32_BLE_Arduino/tree/master/examples
 * Based on https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEScan.h
 */

#define SCAN_TIME 10 // In seconds
#define UUID "00001803-494c-4f47-4934-544543480000" // Manifacturer data of the iBeacon

#include <Arduino.h>
#include <BLEDevice.h>


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str()); // Prints all devices to serial monitor

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(UUID)) {
      Serial.print("Found our device!");
    }
  }
};


BLEScanResults foundDevices() {
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  return pBLEScan->start(SCAN_TIME);
}


void setup() {
  Serial.begin(115200);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Scanning...");
  BLEScanResults devices = foundDevices();
  int count = devices.getCount();
  Serial.println("Devices found: " + count);
  Serial.println("Scan done!");
  delay(2000);
}
