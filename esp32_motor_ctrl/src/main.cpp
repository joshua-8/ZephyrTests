#include <stdio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>

#include "BSED.h"
#include "hbridge.h"

const struct pwm_dt_spec pin1 = PWM_DT_SPEC_GET(DT_ALIAS(pin1));
const struct pwm_dt_spec pin2 = PWM_DT_SPEC_GET(DT_ALIAS(pin2));

HBridge hbridge1 = HBridge(pin1, pin2);

#if DT_NODE_HAS_STATUS(DT_ALIAS(i2c00), okay)
#define I2C_DEV_NODE DT_ALIAS(i2c00)
#else
#error "Please set the correct I2C device"
#endif

const struct device* i2c_dev = DEVICE_DT_GET(I2C_DEV_NODE);

uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;

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

    BSED bsed = BSED(i2c_dev);
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
            printf("Encoder %d: %d\n", i, bsed.getEncoderPosition(i));
        }
    }

    return 0;
}
