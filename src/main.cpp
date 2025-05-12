#include <Arduino.h>

#include <lovense.hpp>


void setup() {
    Serial.begin(115200);
    while (!Serial) delay(100);

    Lovense::init();
}

void loop() {}