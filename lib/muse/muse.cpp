#include "muse.hpp"

#include <NimBLEDevice.h>



namespace Muse {
    const char* TAG = "muse";
    uint16_t MANUFACTURER_ID = 0xFFF0;

    uint8_t _intensity_value = 0;
    uint8_t _last_set_intensity_value = 0;

    bool _stopping = false;



    uint8_t mf_data_list[][MUSE_MF_DATA_LENGTH] = {
        // Stop all channels
        {MUSE_MF_DATA_PREFIX, 0xE5, 0x15, 0x7D},
        // Set all channels to speed 1
        {MUSE_MF_DATA_PREFIX, 0xE4, 0x9C, 0x6C},
        // Set all channels to speed 2
        {MUSE_MF_DATA_PREFIX, 0xE7, 0x07, 0x5E},
        // Set all channels to speed 3
        {MUSE_MF_DATA_PREFIX, 0xE6, 0x8E, 0x4F},
        // Stop 1st channel (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xD5, 0x96, 0x4C},
        // Set 1st channel to speed 1 (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xD4, 0x1F, 0x5D},
        // Set 1st channel to speed 2 (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xD7, 0x84, 0x6F},
        // Set 1st channel to speed 3 (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xD6, 0x0D, 0x7E},
        // Stop 2nd channel (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xA5, 0x11, 0x3F},
        // Set 2nd channel to speed 1 (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xA4, 0x98, 0x2E},
        // Set 2nd channel to speed 2 (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xA7, 0x03, 0x1C},
        // Set 2nd channel to speed 3 (only for toys with 2 channels)
        {MUSE_MF_DATA_PREFIX, 0xA6, 0x8A, 0x0D},
    };



    void set_manufacturer_data(uint8_t index) {
        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

        // Serial.println("Stopping advertisement...");
        pAdvertising->stop();

        uint8_t *manufacturerData = mf_data_list[index];

        pAdvertising->setManufacturerData(std::string((char *)&MANUFACTURER_ID, 2) + std::string((char *)manufacturerData, 11));
        pAdvertising->start();
        
        ESP_LOGD(TAG, "Manufacturer data has been set");
    }

    void advertising_task(void *pvParameters) {
        ESP_LOGD(TAG, "Advertising task started");

        while (!_stopping) {
            // if (_last_set_intensity_value != _intensity_value) {
            set_manufacturer_data(_intensity_value);
            // _last_set_intensity_value = _intensity_value;
            // }
            delay(100);
        }
        
        // advertise stop all channels for a little while
        for (uint8_t i = 0; i < 10; i++) {
            set_manufacturer_data(0);
            delay(200);
        }
        vTaskDelete(NULL);
    }

    void set_intensity_lovense(int lovense_intensity) {
        set_intensity(static_cast<float>(lovense_intensity) / 20.0F);
    }

    void set_intensity(float intensity_percent) {
        // Convert the intensity percent to a value between 0 and 4
        _intensity_value = static_cast<uint8_t>(std::floor(intensity_percent * 4.0f));

        if (intensity_percent < 0.0) {
            Serial.println("Intensity smaller than 0.0, received, cutting at 0.0");
            _intensity_value = 0;
        } else if (intensity_percent > 1.0) {
            Serial.println("Intensity larger than 1.0, received, cutting at 1.0.");
            _intensity_value = 3;
        } else if (isnan(intensity_percent)) {
            Serial.println("Intensity NaN, received, cutting at 0.0");
            _intensity_value = 0;
        }

        if (_intensity_value == 4) _intensity_value = 3;

        Serial.println(String("Percent: ") + intensity_percent + ", MuSe Vibration: " + _intensity_value);
    }

    void muse_init() {
        ESP_LOGD(TAG, "Initializing muse");

        _stopping = false;

        xTaskCreatePinnedToCore(advertising_task, "muse_advertising_task", 4096, nullptr, 2, nullptr, 0);
    }
}