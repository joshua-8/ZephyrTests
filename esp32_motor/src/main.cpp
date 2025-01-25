#include <stdio.h>
#include <zephyr/kernel.h>

#include "hbridge.h"

const struct pwm_dt_spec pin1 = PWM_DT_SPEC_GET(DT_ALIAS(pin1));
const struct pwm_dt_spec pin2 = PWM_DT_SPEC_GET(DT_ALIAS(pin2));

HBridge hbridge1 = HBridge(pin1, pin2);

int main(void)
{
    if (!hbridge1.begin()) {
        printf("Failed to initialize HBridge\n");
        return 1;
    }

    while (true) {
        hbridge1.enable();
        hbridge1.setSpeed(0.7);
        k_sleep(K_MSEC(1000));
        hbridge1.setSpeed(-0.3);
        k_sleep(K_MSEC(1000));
        hbridge1.disable();
        k_sleep(K_MSEC(1000));
    }

    return 0;
}
