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
    radio.receive((char*) buffer);
    
    for (uint8_t i = 0; i < 2; i++) {
        Frame* frame = &buffer[i];
        if (frame->signature[0] != 'V'
                || frame->signature[1] != 'L'
                || frame->signature[2] != 'O'
                || frame->checksum != ace_crc::crc32_nibble::crc_calculate(&frame->data, sizeof(SensedData))) {
            continue;
        }

        const SensedData* data = &frame->data;

        SerialUSB.print(data->index);
        SerialUSB.print('\t');
        SerialUSB.print(data->uptime);
        SerialUSB.print('\t');
        SerialUSB.print(data->micros);
        SerialUSB.print('\t');
        SerialUSB.print(data->temperature);
        SerialUSB.print('\t');
        SerialUSB.print(data->pressure);
        SerialUSB.print('\t');
        SerialUSB.print(data->vibrations);
        SerialUSB.print('\t');
        SerialUSB.print(data->acceleration[0], 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->acceleration[1], 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->acceleration[2], 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->accelerationStatus);
        SerialUSB.print('\t');
        SerialUSB.print(data->gyroscope[0], 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->gyroscope[1], 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->gyroscope[2], 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->gyroscopeStatus);
        SerialUSB.print('\t');
        SerialUSB.print(data->gpsDate);
        SerialUSB.print('\t');
        SerialUSB.print(data->gpsTime);
        SerialUSB.print('\t');
        SerialUSB.print(data->gpsLatitude, 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->gpsLongitude, 6);
        SerialUSB.print('\t');
        SerialUSB.print(data->gpsAltitude, 6);
        SerialUSB.println();

        logFile.print(data->index);
        logFile.print('\t');
        logFile.print(data->uptime);
        logFile.print('\t');
        logFile.print(data->micros);
        logFile.print('\t');
        logFile.print(data->temperature);
        logFile.print('\t');
        logFile.print(data->pressure);
        logFile.print('\t');
        logFile.print(data->vibrations);
        logFile.print('\t');
        logFile.print(data->acceleration[0], 6);
        logFile.print('\t');
        logFile.print(data->acceleration[1], 6);
        logFile.print('\t');
        logFile.print(data->acceleration[2], 6);
        logFile.print('\t');
        logFile.print(data->accelerationStatus);
        logFile.print('\t');
        logFile.print(data->gyroscope[0], 6);
        logFile.print('\t');
        logFile.print(data->gyroscope[1], 6);
        logFile.print('\t');
        logFile.print(data->gyroscope[2], 6);
        logFile.print('\t');
        logFile.print(data->gyroscopeStatus);
        logFile.print('\t');
        logFile.print(data->gpsDate);
        logFile.print('\t');
        logFile.print(data->gpsTime);
        logFile.print('\t');
        logFile.print(data->gpsLatitude, 6);
        logFile.print('\t');
        logFile.print(data->gpsLongitude, 6);
        logFile.print('\t');
        logFile.print(data->gpsAltitude, 6);
        logFile.println();
    }

    if (millis() - lastLogWrite > LOG_WRITE_INTERVAL) {
        logFile.flush();
        lastLogWrite = millis();
    }

}