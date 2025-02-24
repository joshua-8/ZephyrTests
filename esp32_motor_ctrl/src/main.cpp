#include <stdio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/time_units.h>
#include <zephyr/sys/util_macro.h>

#include "BSED.h"
#include "controlLoop.h"
#include "hbridge.h"

const struct pwm_dt_spec pin1 = PWM_DT_SPEC_GET(DT_ALIAS(pin1));
const struct pwm_dt_spec pin2 = PWM_DT_SPEC_GET(DT_ALIAS(pin2));

HBridge hbridge1 = HBridge(pin1, pin2);

#if DT_NODE_HAS_STATUS(DT_ALIAS(i2cport), okay)
#define I2C_DEV_NODE DT_ALIAS(i2cport)
#else
#error "Please set the correct I2C device"
#endif

const struct device* i2c_dev = DEVICE_DT_GET(I2C_DEV_NODE);

uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;

const uint16_t address = 14;

void write(uint8_t data)
{
    i2c_write(i2c_dev, &data, 1, address);
}

int16_t read()
{
    uint8_t data;
    if (i2c_read(i2c_dev, &data, 1, address) == 0) {
        return data;
    } else {
        return -1;
    }
}

uint32_t time()
{
    return (uint64_t)k_cycle_get_32() * 1000000 / CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
}

void delayMicros(uint32_t us)
{
    k_sleep(K_USEC(us));
}

int main(void)
{
    if (!hbridge1.begin()) {
        printf("Failed to initialize HBridge\n");
        return 1;
    }

    uint32_t i2c_cfg_tmp;

    if (!device_is_ready(i2c_dev)) {
        printf("I2C device is not ready\n");
        return 1;
    }

    /* 1. Verify i2c_configure() */
    if (i2c_configure(i2c_dev, i2c_cfg)) {
        printf("I2C config failed\n");
        return 1;
    }

    /* 2. Verify i2c_get_config() */
    if (i2c_get_config(i2c_dev, &i2c_cfg_tmp)) {
        printf("I2C get_config failed\n");
        return 1;
    }
    if (i2c_cfg != i2c_cfg_tmp) {
        printf("I2C get_config returned invalid config\n");
        return 1;
    }

    BSED bsed = BSED(write, read, time, delayMicros);
    bsed.begin();

    hbridge1.enable();

    unsigned long lastTime = time();

    float targetVelocity;

    ControlLoop controlLoop = ControlLoop(0.001, 0.001, 0.00005, 1.0 / 100, 10);

    while (true) {
        unsigned long now = time();
        if (now - lastTime > 10000) { // 100Hz

            // for 5 seconds set targetVelocity to 2000 then for 3 seconds set targetVelocity to -1000
            if((now%8000000) < 5000000){
                targetVelocity = 2000;
            } else {
                targetVelocity = -1000;
            }

            lastTime = now;
            bsed.run();
            float encoder1 = bsed.getEncoderVelocity(1);
            float error = targetVelocity - encoder1;
            float output = 0;
            output = controlLoop.calculate(error);
            printf("Encoder1: %f, Error: %f, Output: %f\n", (double)encoder1, (double)error, (double)output);
            hbridge1.setSpeed(output);
        }
    }

    return 0;
}
