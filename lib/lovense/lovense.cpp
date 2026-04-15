#include "lovense.hpp"

#include <muse.hpp>


namespace Lovense {
    NimBLECharacteristic *pCharRx = nullptr;
    NimBLECharacteristic *pCharTx = nullptr;


    void ServerCallback::onConnect(NimBLEServer *pServer) {
        Serial.println("A device connected to the Lovense interface.");
    }

    void ServerCallback::onDisconnect(NimBLEServer *pServer) {
        Serial.println("A device disconnected from the Lovense interface.");
    }


    void CharCallback::onWrite(NimBLECharacteristic *pChar) {
        if (pChar != pCharTx) {
            Serial.println("Wrong characteristic.");
            return;
        }

        Serial.println("=== REQUEST RECEIVED TO LOVENSE INTERFACE ===");

        std::string request = pChar->getValue();
        Serial.println(request.c_str());

        std::string response = "";

        if (request == "DeviceType;") {
            Serial.println("Responding with device type...");

            std::string firmwareVersion = "11";
            std::string deviceType = "S";
            uint8_t mac[6];
            esp_read_mac(mac, ESP_MAC_WIFI_STA);
            char macAddressStr[18];
            sprintf(macAddressStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            response = deviceType + ":" + firmwareVersion + ":" + macAddressStr + ";";
        }
        else if (request.rfind("Vibrate:", 0) == 0) {
            Serial.println("Setting vibration level...");

            size_t startPos = request.find(":") + 1;
            size_t endPos = request.find(";");

            if (startPos == std::string::npos || endPos == std::string::npos || endPos <= startPos) {
                Serial.println("Invalid level.");
                return;
            }

            int vibration = std::stoi(request.substr(startPos, endPos - startPos));
            Serial.println(String("Received vibration level: ") + vibration);

            Muse::set_intensity_lovense(vibration);
        }
        else if (request == "Battery;") {
            response = "69;";
        }
        else {
            Serial.println("Unknown request.");
            response = "ERR;";
        }

        Serial.println(String("Responding with payload: ") + response.c_str());
        pCharRx->setValue(response);
        pCharRx->notify();
    }


    void init() {
        Serial.println("=== Initializing BLE Server for Lovense ===");
        Serial.println(String("Name: ") + LOVENSE_NAME);

        NimBLEDevice::init(LOVENSE_NAME);

        Serial.println("Creating server...");
        NimBLEServer *pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(new ServerCallback());

        Serial.println(String("Creating service with UUID ") + LOVENSE_SERVICE_UUID);
        NimBLEService *pService = pServer->createService(LOVENSE_SERVICE_UUID);


        Serial.println(String("Creating RX characteristic with UUID ") + LOVENSE_CHAR_RX_UUID);
        pCharRx = pService->createCharacteristic(
            LOVENSE_CHAR_RX_UUID,
            NIMBLE_PROPERTY::NOTIFY
        );
        // pCharRx->addDescriptor(new BLE2902());
        pCharRx->setCallbacks(new CharCallback());
    
        Serial.println(String("Creating TX characteristic with UUID ") + LOVENSE_CHAR_TX_UUID);
        pCharTx = pService->createCharacteristic(
            LOVENSE_CHAR_TX_UUID,
            NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
        );
        pCharTx->setCallbacks(new CharCallback());
        

        Serial.println("Starting service...");
        pService->start();


        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

        pAdvertising->addServiceUUID(LOVENSE_SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x0);

        Serial.println("Starting advertising...");
        NimBLEDevice::startAdvertising();

        Serial.println("Initialized. Waiting for connection.");
    }
}