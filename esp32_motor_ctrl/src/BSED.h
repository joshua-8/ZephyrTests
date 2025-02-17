#ifndef BSED_H
#define BSED_H
#include <cstdint>

class BSED {
private:
    const struct device* i2c_dev;
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

    inline void write(uint8_t data);
    inline int16_t read();

public:
    BSED(const struct device* _i2c_dev, uint8_t _address = 14, int16_t _encoderSlowestInterval = 0, int16_t _encoderEnoughCounts = 0);

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
