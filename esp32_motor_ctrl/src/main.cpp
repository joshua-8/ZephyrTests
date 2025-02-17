#include <stdio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/time_units.h>
#include <zephyr/sys/util_macro.h>

#include "BSED.h"
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

    while (true) {
        // hbridge1.enable();
        // hbridge1.setSpeed(0.7);
        k_sleep(K_MSEC(300));
        // hbridge1.setSpeed(-0.3);
        // k_sleep(K_MSEC(1000));
        // hbridge1.disable();
        // k_sleep(K_MSEC(1000));
        bsed.run();

        for (int i = 1; i <= 8; i++) {
            printf("Encoder %d: %d\n", i, bsed.getEncoderVelocity(i));
        }
        printf("\n");
    }

    return 0;
}
