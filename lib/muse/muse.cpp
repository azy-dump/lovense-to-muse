#include "muse.hpp"

#include <NimBLEDevice.h>



namespace Muse {
    const char* TAG = "muse";
    uint16_t MANUFACTURER_ID = 0xFFF0;

    // uint8_t _intensity_value = 0;

    bool _stopping = false;



    uint8_t mf_data_list[][MUSE_MF_DATA_LENGTH] = {
        // Stop
        {MUSE_MF_DATA_PREFIX, 0xe5, 0x0, 0x0},

        // 1 - 9
        {MUSE_MF_DATA_PREFIX, 0xf4, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xf7, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xf6, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xf1, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xf0, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xf3, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xe7, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xfc, 0x0, 0x0},
        {MUSE_MF_DATA_PREFIX, 0xe6, 0x0, 0x0},
    };

    int previous_muse_intensity = 0;
    int current_muse_intensity = 0;

    int lovense_intensity_max = 20;
    int muse_intensity_max = 9;


    void set_manufacturer_data(uint8_t index) {
        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

        // Serial.println("Stopping advertisement...");
        pAdvertising->stop();

        uint8_t *manufacturerData = mf_data_list[index];

        pAdvertising->setManufacturerData(std::string((char *)&MANUFACTURER_ID, 2) + std::string((char *)manufacturerData, 11));
        pAdvertising->start();
        
        ESP_LOGD(TAG, "Manufacturer data has been set");
    }

    void tick() {
        if (previous_muse_intensity == current_muse_intensity) return;

        set_manufacturer_data(current_muse_intensity);
        previous_muse_intensity = current_muse_intensity;

        // Serial.println(String("S: ") + upper_stopwatch_max + " " + upper_stopwatch + " | " + lower_stopwatch + " " + lower_stopwatch_max + " ||| " + is_using_upper);
    }

    void advertising_task(void *pvParameters) {
        ESP_LOGD(TAG, "Advertising task started");

        while (!_stopping) {
            tick();
            delay(20);
            // set_manufacturer_data(1);
            // delay(100);
        }
        
        // advertise stop all channels for a little while
        for (uint8_t i = 0; i < 10; i++) {
            set_manufacturer_data(0);
            delay(200);
        }
        vTaskDelete(NULL);
    }

    void set_intensity_lovense(int lovense_intensity) {
        if (lovense_intensity < 0) {
            Serial.println("Intensity smaller than 0 received! Setting back to 0.");
            lovense_intensity = 0;
        } else if (lovense_intensity > lovense_intensity_max) {
            Serial.println(String("Intensity greater than ") + lovense_intensity_max + " received! Setting to max.");
            lovense_intensity = lovense_intensity_max;
        } else if (isnan(lovense_intensity)) {
            Serial.println("Intensity NaN received, not doing anything.");
            return;
        }

        Serial.println(String("Setting intensity to: ") + lovense_intensity);

        if (lovense_intensity == lovense_intensity_max) {
            current_muse_intensity = muse_intensity_max;
            return;
        }
        if (lovense_intensity == 0) {
            current_muse_intensity = 0;
            return;
        }

        float progress = (static_cast<float>(lovense_intensity) / static_cast<float>(lovense_intensity_max));
        current_muse_intensity = std::floor(progress * muse_intensity_max);
    }

    // void set_intensity(float intensity_percent) {
    //     // Convert the intensity percent to a value between 0 and 4
    //     _intensity_value = static_cast<uint8_t>(std::floor(intensity_percent * 4.0f));

    //     if (intensity_percent < 0.0) {
    //         Serial.println("Intensity smaller than 0.0, received, cutting at 0.0");
    //         _intensity_value = 0;
    //     } else if (intensity_percent > 1.0) {
    //         Serial.println("Intensity larger than 1.0, received, cutting at 1.0.");
    //         _intensity_value = 3;
    //     } else if (isnan(intensity_percent)) {
    //         Serial.println("Intensity NaN, received, cutting at 0.0");
    //         _intensity_value = 0;
    //     }

    //     if (_intensity_value == 4) _intensity_value = 3;

    //     Serial.println(String("Percent: ") + intensity_percent + ", MuSe Vibration: " + _intensity_value);
    // }

    void muse_init() {
        ESP_LOGD(TAG, "Initializing muse");

        _stopping = false;

        xTaskCreatePinnedToCore(advertising_task, "muse_advertising_task", 4096, nullptr, 2, nullptr, 0);
    }
}