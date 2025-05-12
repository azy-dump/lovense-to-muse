#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


#define MUSE_MF_DATA_LENGTH 11
#define MUSE_MF_DATA_PREFIX 0x6D, 0xB6, 0x43, 0xCE, 0x97, 0xFE, 0x42, 0x7C



namespace Muse {
    extern const char* TAG;
    extern uint16_t MANUFACTURER_ID;

    extern uint8_t _intensity_value;
    extern uint8_t _last_set_intensity_value;

    extern bool _stopping;

    extern uint8_t mf_data_list[][MUSE_MF_DATA_LENGTH];



    void set_manufacturer_data(uint8_t index);
    void advertising_task(void *pvParameters);

    void set_intensity_lovense(int lovense_intensity);
    void set_intensity(float intensity_percent);

    void muse_init();
}












