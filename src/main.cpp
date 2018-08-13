/**
 * BLE client to scan and identify iBeacons.
 * Based on Neil Kolban's examples on https://github.com/nkolban/ESP32_BLE_Arduino/tree/master/examples
 * Based on https://github.com/moononournation/Arduino_BLE_Scanner
 */

#define SCAN_TIME 5 // In seconds

#include <Arduino.h>
#include <BLEDevice.h>


struct deviceInfo {
  String address;
  unsigned int maxDistance;
  bool hasBeenScanned;
  bool IsInRange;
};

int myDevicesCount = 1; // Number of devices we are looking for
deviceInfo myDevices[] = {{"5c:f8:21:dd:f0:db", 1, false, false}}; // Radioland iBeacon

BLEAdvertisedDevice* myBeacon = NULL;


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // We have found a device, let us now see if it contains the service we are looking for.

    // Serial.printf("Advertised Device: %s\n", advertisedDevice.toString().c_str()); // Prints all devices to serial monitor

    String deviceAddress = advertisedDevice.getAddress().toString().c_str();

    for (int i = 0; i < myDevicesCount; i++) {
      if (deviceAddress == myDevices[i].address) {
        myDevices[i].hasBeenScanned = true;

        Serial.print("Found our device! ");
        Serial.printf("Address: ");
        Serial.println(deviceAddress);

        int rssi = advertisedDevice.getRSSI();
        unsigned int distance = rssiInMeter(rssi);
        Serial.printf("RSSI: %d\tDistance: %dm\n", rssi, distance);

        if (distance <= myDevices[i].maxDistance) {
          myDevices[i].IsInRange = true;
        }
      }
    }
  }

  int rssiInMeter(int rssi) {
      int mpower = -72;
      float n = 2.0; // Constant depends on the Environmental factor. Range 2-4
      return pow(10, (mpower - rssi)/(10 * n));
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
  Serial.printf("Done! Devices found: %d\n", count);

  for (int i = 0; i < myDevicesCount; i++) {
    if (myDevices[i].hasBeenScanned == false || myDevices[i].IsInRange == false) {
      Serial.printf("\nALERT!: Device out of range: ");
      Serial.println(myDevices[i].address);
    }
  }

  Serial.println();
}
