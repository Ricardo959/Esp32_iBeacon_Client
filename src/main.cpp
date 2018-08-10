/**
 * BLE client to scan and identify iBeacons.
 * Based on Neil Kolban's examples on https://github.com/nkolban/ESP32_BLE_Arduino/tree/master/examples
 * Based on https://github.com/nkolban/ESP32_BLE_Arduino/blob/master/src/BLEScan.h
 */

#define SCAN_TIME 5 // In seconds

#include <Arduino.h>
#include <BLEDevice.h>

static String ADDRESS = "5c:f8:21:dd:f0:db"; // Radioland iBeacon

BLEAdvertisedDevice* myBeacon = NULL;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // We have found a device, let us now see if it contains the service we are looking for.

    // Serial.printf("Advertised Device: %s\n", advertisedDevice.toString().c_str()); // Prints all devices to serial monitor

    String deviceAddress = advertisedDevice.getAddress().toString().c_str();

    if (deviceAddress == ADDRESS) {
      Serial.print("Found our device! ");
      Serial.printf("Address: ");
      Serial.println(deviceAddress);
      Serial.printf("RSSI: %d\n", advertisedDevice.getRSSI());
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
  Serial.printf("Done! Devices found: %d\n\n", count);
  delay(100);
}
