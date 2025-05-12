#pragma once

#include <Arduino.h>

#include <NimBLEDevice.h>


// Currently emulating the Lovense Lush (v1).

#define LOVENSE_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_TX_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define LOVENSE_CHAR_RX_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

#define LOVENSE_NAME "LVS-LoveSpouse01"



namespace Lovense {
    // oh my goodness it's SUPPOSED to be like this, it took me way too long cause it's BACKWARDS :((
    // RX = where you SEND data
    // TX = where you RECEIVE data
    extern NimBLECharacteristic *pCharRx;
    extern NimBLECharacteristic *pCharTx;


    class ServerCallback : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer *pServer) override;
        void onDisconnect(NimBLEServer *pServer) override;
    };


    class CharCallback : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic *pChar);
    };


    void init();
}