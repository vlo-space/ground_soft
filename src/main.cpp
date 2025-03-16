#include <Arduino.h>

#include <CanSatKit.h>
#include <CanSatKitRadio.h>
#include <SD.h>
#include <ace_crc/crc32_nibble.hpp>

#include "data.h"
#include "pins.h"

#define LOG_WRITE_INTERVAL 1000

CanSatKit::Radio radio( 7,
                        6,
                        433.0,
                        CanSatKit::Bandwidth_500000_Hz,
                        CanSatKit::SpreadingFactor_7,
                        CanSatKit::CodingRate_4_8);

SDLib::File logFile;

uint32_t lastLogWrite = 0;

void setup() {
    SerialUSB.begin(115200);

    if (!radio.begin()) {
        while (true) {
            digitalWrite(PIN_LED, true);
            delay(1000);
            digitalWrite(PIN_LED, false);
            delay(1000);
        }
    }

    if (SD.begin(PIN_SD_SELECT)) {
        logFile = SD.open("latest.log", O_APPEND | O_CREAT | O_WRITE);
        logFile.print('\n');
        logFile.println("--- STARTUP ---");
        logFile.flush();
    }
}

void loop() {

    Frame buffer[2] = {0};
    digitalWrite(PIN_LED, true);
    radio.receive((char*) buffer);
    digitalWrite(PIN_LED, false);
    
    for (uint8_t i = 0; i < 2; i++) {
        Frame* frame = &buffer[i];
        if (frame->signature[0] != 'V'
                || frame->signature[1] != 'L'
                || frame->signature[2] != 'O'
                || frame->checksum != ace_crc::crc32_nibble::crc_calculate(&frame->data, sizeof(SensedData))) {
            continue;
        }

        const SensedData* data = &frame->data;

        data->print(&SerialUSB);
        data->print(&logFile);
    }

    if (millis() - lastLogWrite > LOG_WRITE_INTERVAL) {
        logFile.flush();
        lastLogWrite = millis();
    }

}