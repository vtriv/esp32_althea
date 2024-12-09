#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs for the service and characteristic
#define SERVICE_UUID        "180D"
#define CHARACTERISTIC_UUID "2A56"

#define LED_POWER           9   // led for power to device
#define LED_CONNECT         10   // led to show device is connected
#define LED_INPROGRESS      11   // led to show treatment in progress // RGB_BUILTIN also shows this
#define SQUARE_WAVE_PIN     14  // output for 1.7 MHz square wave

// Variables to track states
bool state = false;            // initially off

// Callback class to handle BLE server events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Device connected!");
    digitalWrite(LED_CONNECT, HIGH);
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Device disconnected!");
    digitalWrite(LED_CONNECT, LOW);
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
      // Serial.print("Received data: ");
      // Serial.println(value.c_str());
      
      state = !state; // Toggle state
      digitalWrite(RGB_BUILTIN, state ? HIGH : LOW);
      digitalWrite(LED_INPROGRESS, state ? HIGH : LOW);

      if (state) {
        ledcWrite(SQUARE_WAVE_PIN, 1);               // Start the ultrasound signal
        Serial.println("ultrasound should be triggered");
      } else {
        ledcWrite(SQUARE_WAVE_PIN, 0);               // Stop the ultrasound signal
        Serial.println("ultrasound should be off");
      }
      // moved rest to void loop so that it doesn't get in the way of ultrasound output
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

  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_CONNECT, OUTPUT);
  pinMode(LED_INPROGRESS, OUTPUT);
  pinMode(SQUARE_WAVE_PIN, OUTPUT);

  digitalWrite(LED_POWER, HIGH); // turn on LED to indicate power to device
  ledcDetach(SQUARE_WAVE_PIN);
  ledcAttach(SQUARE_WAVE_PIN, 1770000, 1);
}

void loop() {
  delay(1000);
}