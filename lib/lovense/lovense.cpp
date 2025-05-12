#include "lovense.hpp"



namespace Lovense {
    BLECharacteristic *pCharRx = nullptr;
    BLECharacteristic *pCharTx = nullptr;


    void ServerCallback::onConnect(BLEServer *pServer) {
        Serial.println("A device connected to the Lovense interface.");
    }

    void ServerCallback::onDisconnect(BLEServer *pServer) {
        Serial.println("A device disconnected from the Lovense interface.");
    }


    void CharCallback::onWrite(BLECharacteristic *pChar) {
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


    void init() {
        Serial.println("=== Initializing BLE Server for Lovense ===");
        Serial.println(String("Name: ") + LOVENSE_NAME);

        BLEDevice::init(LOVENSE_NAME);

        Serial.println("Creating server...");
        BLEServer *pServer = BLEDevice::createServer();
        pServer->setCallbacks(new ServerCallback());

        Serial.println(String("Creating service with UUID ") + LOVENSE_SERVICE_UUID);
        BLEService *pService = pServer->createService(LOVENSE_SERVICE_UUID);


        Serial.println(String("Creating RX characteristic with UUID ") + LOVENSE_CHAR_RX_UUID);
        pCharRx = pService->createCharacteristic(
            LOVENSE_CHAR_RX_UUID,
            BLECharacteristic::PROPERTY_NOTIFY
        );
        pCharRx->addDescriptor(new BLE2902());
        pCharRx->setCallbacks(new CharCallback());
    
        Serial.println(String("Creating TX characteristic with UUID ") + LOVENSE_CHAR_TX_UUID);
        pCharTx = pService->createCharacteristic(
            LOVENSE_CHAR_TX_UUID,
            BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
        );
        pCharTx->setCallbacks(new CharCallback());
        

        Serial.println("Starting service...");
        pService->start();


        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

        pAdvertising->addServiceUUID(LOVENSE_SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x0);

        Serial.println("Starting advertising...");
        BLEDevice::startAdvertising();

        Serial.println("Initialized. Waiting for connection.");
    }
}