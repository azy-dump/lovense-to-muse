#pragma once

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLE2902.h>


// Currently emulating the Lovense Lush (v1).

#define LOVENSE_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_TX_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_RX_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

#define LOVENSE_NAME "LVS-LoveSpouse01"



namespace Lovense {
    // oh my goodness it's SUPPOSED to be like this, it took me way too long cause it's BACKWARDS :((
    // RX = where you SEND data
    // TX = where you RECEIVE data
    extern BLECharacteristic *pCharRx;
    extern BLECharacteristic *pCharTx;


    class ServerCallback : public BLEServerCallbacks {
        void onConnect(BLEServer *pServer) override;
        void onDisconnect(BLEServer *pServer) override;
    };


    class CharCallback : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pChar);
    };


    void init();
}