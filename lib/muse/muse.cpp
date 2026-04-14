#include "muse.hpp"

#include <NimBLEDevice.h>



namespace Muse {
    const char* TAG = "muse";
    uint16_t MANUFACTURER_ID = 0xFFF0;

    // uint8_t _intensity_value = 0;

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


    int lower_intensity_step = 0;
    int upper_intensity_step = 1;
    float upper_length_percent = 0.0F;

    int loop_length_ticks = 5;
    int upper_stopwatch = 0; // Runs when the upper intensity step is active
    int lower_stopwatch = 0; // Runs when the lower intensity step is active
    bool is_using_upper = true;

    int current_lovense_intensity = 0;

    int lovense_intensity_max = 20;
    int muse_intensity_max = 3;


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
        int upper_stopwatch_max = floor(loop_length_ticks * upper_length_percent);
        int lower_stopwatch_max = loop_length_ticks - upper_stopwatch_max;

        if (is_using_upper) {
            upper_stopwatch++;
            if (upper_stopwatch > upper_stopwatch_max) {
                upper_stopwatch = 0;
                is_using_upper = false;
            } else {
                set_manufacturer_data(upper_intensity_step);
            }
        } else {
            lower_stopwatch++;
            if (lower_stopwatch > lower_stopwatch_max) {
                lower_stopwatch = 0;
                is_using_upper = true;
            } else {
                set_manufacturer_data(lower_intensity_step);
            }
        }

        // Serial.println(String("S: ") + upper_stopwatch_max + " " + upper_stopwatch + " | " + lower_stopwatch + " " + lower_stopwatch_max + " ||| " + is_using_upper);
    }

    void advertising_task(void *pvParameters) {
        ESP_LOGD(TAG, "Advertising task started");

        while (!_stopping) {
            tick();
            delay(70);
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
            lower_intensity_step = muse_intensity_max;
            upper_intensity_step = muse_intensity_max;
            upper_length_percent = 1.0F;
            return;
        }

        float progress = (static_cast<float>(lovense_intensity) / static_cast<float>(lovense_intensity_max)) * muse_intensity_max;
        lower_intensity_step = std::floor(progress);
        upper_intensity_step = lower_intensity_step + 1;
        upper_length_percent = progress - lower_intensity_step;


        
        // set_intensity(static_cast<float>(lovense_intensity) / 20.0F);
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