#include <Arduino.h>

#include <BLEDevice.h>
#include <BLE2902.h>


#define LOVENSE_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_TX_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_RX_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

#define LOVENSE_NAME "LVS-S001"


static BLECharacteristic *pCharRx = nullptr;
static BLECharacteristic *pCharTx = nullptr;


class ServerCallback : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) override {
        Serial.println("A device connected to the Lovense interface.");
    }

    void onDisconnect(BLEServer *pServer) override {
        Serial.println("A device disconnected from the Lovense interface.");
    }
};


class CharCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
        if (pChar != pCharTx) {
            Serial.println("Wrong characteristic.");
            return;
        }

        std::string input = pChar->getValue();
        Serial.println(input.c_str());

        if (input == "DeviceType;") {
            Serial.println("$Responding to Device Enquiry");
            std::string wawa = "S:11:004B123A2E62;";
            pCharRx->setValue(wawa);
            pCharRx->notify();
        } else {
            Serial.println("$Wawa");
            std::string wawa = "ERR;";
            pCharRx->setValue(wawa);
            pCharRx->notify();
        }
    }
};


void setup() {
    Serial.begin(115200);
    while (!Serial) delay(100);


    Serial.println("Initializing BLE...");


    BLEDevice::init(LOVENSE_NAME);

    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallback());

    BLEService *pService = pServer->createService(LOVENSE_SERVICE_UUID);


    pCharRx = pService->createCharacteristic(
        LOVENSE_CHAR_RX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharRx->addDescriptor(new BLE2902());
    pCharRx->setCallbacks(new CharCallback());
  
    pCharTx = pService->createCharacteristic(
        LOVENSE_CHAR_TX_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
    );
    pCharTx->setCallbacks(new CharCallback());
    


    pService->start();


    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(LOVENSE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x0);
    // pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Initialized. Waiting for connection.");
}

void loop() {}