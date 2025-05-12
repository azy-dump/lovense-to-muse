#include <Arduino.h>

#include <lovense.hpp>
#include <muse.hpp>


void setup() {
    Serial.begin(115200);
    while (!Serial) delay(100);


    Lovense::init();
    Muse::muse_init();
}

void loop() {}