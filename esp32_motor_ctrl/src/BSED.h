#ifndef BSED_H
#define BSED_H
#include <cstdint>

class BSED {
protected:
    uint8_t address;
    uint8_t whichEncodersMask;
    int16_t encoderCount[8];
    int16_t encoderOverflows[8];
    int16_t lastEncoderCount[8];
    int16_t lastVelocityEncoderCount[8];
    uint32_t lastReadMicros[8];
    int16_t encoderVelocity[8];
    int16_t encoderSlowestInterval[8];
    int16_t encoderEnoughCounts[8];
    bool isVelNewVal[8];

    void (*write)(uint8_t);
    int16_t (*read)();
    uint32_t (*time)();
    void (*delayMicros)(uint32_t);

public:
    BSED(void _write(uint8_t), int16_t _read(), uint32_t _time(), void _delayMicros(uint32_t), int16_t _encoderSlowestInterval = 0, int16_t _encoderEnoughCounts = 0);

    void setEncoderSlowestInterval(uint8_t n, int16_t interval);
    void setEncoderEnoughCounts(uint8_t n, int16_t counts);
    bool isVelNew(uint8_t n);
    void begin(bool resetEncoders = true);
    void run();
    int32_t getEncoderPosition(uint8_t n, bool read = false);
    int16_t getEncoderPositionWithoutOverflows(uint8_t n, bool read = false);
    int16_t getEncoderVelocity(uint8_t n, bool read = false);
    void resetEncoderPositions(bool resetVariables = true);
    void setWhichEncoders(uint8_t mask);
    bool isEncoderActive(uint8_t n);
};

#endif // BSED_H
