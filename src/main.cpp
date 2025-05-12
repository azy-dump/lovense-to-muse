#include <Arduino.h>

#include <BLEDevice.h>
#include <BLE2902.h>


#define LOVENSE_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_TX_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_RX_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

#define LOVENSE_NAME "LVS-S001"


void setup() {
    Serial.begin(115200);
    while (!Serial) delay(100);


    Serial.println("Initializing BLE...");
    BLEDevice::init(LOVENSE_NAME);
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(LOVENSE_SERVICE_UUID);


    BLECharacteristic *pCharTx = pService->createCharacteristic(
        LOVENSE_CHAR_TX_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    // pCharacteristicTX->setCallbacks(new LovenseCallbacks());

    BLECharacteristic *pCharRx = pService->createCharacteristic(
        LOVENSE_CHAR_RX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharRx->addDescriptor(new BLE2902());



    pService->start();


    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(LOVENSE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {}