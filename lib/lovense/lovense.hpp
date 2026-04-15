#pragma once

#include <Arduino.h>

#include <NimBLEDevice.h>


// Currently emulating the Lovense Lush (v1).

#define LOVENSE_SERVICE_UUID "45440001-0023-4BD4-BBD5-A6920E4C5653"
#define LOVENSE_CHAR_TX_UUID "45440003-0023-4BD4-BBD5-A6920E4C5653"
#define LOVENSE_CHAR_RX_UUID "45440002-0023-4BD4-BBD5-A6920E4C5653"

#define LOVENSE_NAME "LVS-Lush11"



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