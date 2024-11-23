#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs for the service and characteristic
#define SERVICE_UUID        "180D"
#define CHARACTERISTIC_UUID "2A56"

// Callback class to handle BLE server events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Device connected!");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Device disconnected!");
    // Restart advertising so other devices can connect
    pServer->getAdvertising()->start();
    Serial.println("Advertising restarted...");
  }
};

// Callback class to handle characteristic write events
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    // Get the value written to the characteristic
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(value.c_str());
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("ESP32_althea"); // Advertised device name

  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // Set callbacks for connection/disconnection

  // Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristic
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  // Set the characteristic callbacks to handle data writes
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising with the device name
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
  
  Serial.println("BLE device is advertising as ESP32_althea...");
}

void loop() {
  // Nothing to do in loop
  delay(1000);
}