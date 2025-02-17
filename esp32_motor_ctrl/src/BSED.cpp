#include "BSED.h"
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define READ_IS_BIT_SET(value, bit) ((((value) >> (bit)) & (0x1)) != 0)

// void write(uint8_t data)
// {
//     i2c_write(i2c_dev, &data, 1, address);
// }
// int16_t read()
// {
//     uint8_t data;
//     if (i2c_read(i2c_dev, &data, 1, address) == 0) {
//         return data;
//     } else {
//         return -1;
//     }
// }
// uint32_t time()
// {
//     return k_ticks_to_us_floor32(k_cycle_get_32());
// }
// void delayMicros(uint32_t us)
// {
//     k_sleep(K_USEC(us));
// }

BSED::BSED(void _write(uint8_t), int16_t _read(), uint32_t _time(), void _delayMicros(uint32_t), int16_t _encoderSlowestInterval, int16_t _encoderEnoughCounts)
{
    write = _write;
    read = _read;
    time = _time;
    delayMicros = _delayMicros;

    whichEncodersMask = 255;

    // memset encoderCount[] all 0
    memset(encoderCount, 0, 8 * sizeof(int16_t));
    memset(encoderOverflows, 0, 8 * sizeof(int16_t));
    memset(lastEncoderCount, 0, 8 * sizeof(int16_t));
    memset(lastVelocityEncoderCount, 0, 8 * sizeof(int16_t));
    memset(lastReadMicros, 0, 8 * sizeof(unsigned long));
    memset(encoderVelocity, 0, 8 * sizeof(int16_t));
    memset(encoderSlowestInterval, _encoderSlowestInterval, 8 * sizeof(int16_t));
    memset(encoderEnoughCounts, _encoderEnoughCounts, 8 * sizeof(int16_t));
    memset(isVelNewVal, 0, 8 * sizeof(bool));
}

void BSED::setEncoderSlowestInterval(uint8_t n, int16_t interval)
{
    if (n > 8) {
        return;
    }
    if (n == 0) {
        for (uint8_t i = 0; i < 8; i++) {
            encoderSlowestInterval[i] = interval;
        }
        return;
    }
    encoderSlowestInterval[n - 1] = interval;
}

void BSED::setEncoderEnoughCounts(uint8_t n, int16_t counts)
{
    if (n > 8) {
        return;
    }
    if (n == 0) {
        for (uint8_t i = 0; i < 8; i++) {
            encoderEnoughCounts[i] = counts;
        }
        return;
    }
    encoderEnoughCounts[n - 1] = counts;
}

bool BSED::isVelNew(uint8_t n)
{
    if (n > 8 || n < 1) {
        return false;
    }
    bool temp = isVelNewVal[n - 1];
    isVelNewVal[n - 1] = false;
    return temp;
}

void BSED::begin(bool resetEncoders)
{
    setWhichEncoders(255);
    if (resetEncoders) {
        resetEncoderPositions();
    }
}

void BSED::run()
{
    write(whichEncodersMask);
    delayMicros(50); // time for the board to prepare to respond
    for (uint8_t i = 0; i < 8; i++) {
        if (READ_IS_BIT_SET(whichEncodersMask, 7 - i) == 0) {
            continue;
        }
        int high = -1;
        int low = -1;
        high = read();
        low = read();
        if (high == -1 || low == -1) {
            // we didn't get data
        } else {
            lastEncoderCount[i] = encoderCount[i];
            encoderCount[i] = (((uint16_t)high) << 8 | ((uint16_t)low));
            if (abs(encoderCount[i] - lastEncoderCount[i]) > (1 << 15)) {
                encoderOverflows[i] += (encoderCount[i] > lastEncoderCount[i]) ? -1 : 1;
            }
            // calculate velocity
            uint32_t mic = time();
            int32_t hundredMicrosSinceLastRead = (mic - lastReadMicros[i]) / 100; // using a time interval of 100 microseconds (won't overflow int32)
            if (hundredMicrosSinceLastRead > 0 && (hundredMicrosSinceLastRead > (int32_t)(encoderSlowestInterval[i] * 10) || abs(encoderCount[i] - lastVelocityEncoderCount[i]) > encoderEnoughCounts[i])) {
                lastReadMicros[i] = mic;
                encoderVelocity[i] = (int32_t)10000 * (encoderCount[i] - lastVelocityEncoderCount[i]) / hundredMicrosSinceLastRead;
                lastVelocityEncoderCount[i] = encoderCount[i];
                isVelNewVal[i] = true;
            }
        }
    }
}

int32_t BSED::getEncoderPosition(uint8_t n, bool read)
{
    if (read) {
        run();
    }
    if (n > 8 || n < 1) {
        return 0;
    }
    return (int32_t)encoderCount[n - 1] + (int32_t)encoderOverflows[n - 1] * 65536;
}

int16_t BSED::getEncoderPositionWithoutOverflows(uint8_t n, bool read)
{
    if (read) {
        run();
    }
    if (n > 8 || n < 1) {
        return 0;
    }
    return encoderCount[n - 1];
}

int16_t BSED::getEncoderVelocity(uint8_t n, bool read)
{
    if (read) {
        run();
    }
    if (n > 8 || n < 1) {
        return 0;
    }
    return encoderVelocity[n - 1];
}

void BSED::resetEncoderPositions(bool resetVariables)
{
    write(0);
    if (resetVariables) {
        for (uint8_t i = 0; i < 8; i++) {
            encoderCount[i] = 0;
            encoderOverflows[i] = 0;
            lastEncoderCount[i] = 0;
            encoderVelocity[i] = 0;
        }
    }
}

void BSED::setWhichEncoders(uint8_t mask)
{
    if (mask != 0) {
        whichEncodersMask = mask;
        write(mask);
    }
}

bool BSED::isEncoderActive(uint8_t n)
{
    if (n > 8 || n < 1) {
        return false;
    }
    return READ_IS_BIT_SET(whichEncodersMask, 7 - (n - 1));
}