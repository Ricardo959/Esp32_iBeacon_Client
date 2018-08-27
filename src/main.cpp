/**
 * BLE client to scan and identify iBeacons.
 * Based on Neil Kolban's examples on https://github.com/nkolban/ESP32_BLE_Arduino/tree/master/examples
 * Based on https://github.com/moononournation/Arduino_BLE_Scanner
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <sstream>

#define true 1
#define false 0


static int SCAN_TIME = 5; // In seconds

struct deviceInfo { // Contains all the info we need of each device
  String address;
  unsigned int maxDistance;
  unsigned int minDistance;
  bool isInRange; // Is beeing detectet by our anchor
  bool isInArea; // Is in the area it should bee
};

int myDevicesCount = 2; // Number of devices we are looking for
deviceInfo myDevices[] = {{"5c:f8:21:dd:f0:db", 2, 0}, // Radioland iBeacon
                          {"c0:1b:38:b1:a3:64", 8, 0}}; // M2 mysterious object

BLEAdvertisedDevice* myBeacon = NULL;


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // We have found a device, let us now see if it contains the service we are looking for:

    // Serial.printf("Advertised Device: %s\n", advertisedDevice.toString().c_str()); // Prints all devices to serial monitor

    String deviceAddress = advertisedDevice.getAddress().toString().c_str();

    for (int i = 0; i < myDevicesCount; i++) {
      if (deviceAddress == myDevices[i].address) {
        myDevices[i].isInRange = true;

        Serial.print("Found our device! ");
        Serial.printf("Address: ");
        Serial.println(deviceAddress);

        int rssi = advertisedDevice.getRSSI();
        unsigned int distance = rssiInMeter(rssi);
        Serial.printf("RSSI: %d\tDistance: %d m\n", rssi, distance);

        if (distance <= myDevices[i].maxDistance) {
          myDevices[i].isInArea = true;
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
  BLEScan* pBLEScan = BLEDevice::getScan(); // Create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Active scan uses more power, but get results faster
  return pBLEScan->start(SCAN_TIME);
}


void setup() {
  Serial.begin(115200);
}


void loop() {
  // Put your main code here, to run repeatedly:
  for (int i = 0; i < myDevicesCount; i++) {
    myDevices[i].isInRange = false;
    myDevices[i].isInArea = false;
  }

  Serial.println("Scanning...");
  BLEScanResults devices = foundDevices();
  int count = devices.getCount();
  Serial.printf("Done! Devices found: %d\n\n", count);

  for (int i = 0; i < myDevicesCount; i++) {
    if (!myDevices[i].isInRange) {
      Serial.printf("WARNING!: Device is missing: ");
      Serial.println(myDevices[i].address);
    } else if (!myDevices[i].isInArea) {
      Serial.printf("WARNING!: Device out of area: ");
      Serial.println(myDevices[i].address);
    }
  }

  Serial.println();
}
