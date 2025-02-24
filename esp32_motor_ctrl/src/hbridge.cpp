#include "hbridge.h"
#include <stdio.h>

HBridge::HBridge(const struct pwm_dt_spec& pin1, const struct pwm_dt_spec& pin2)
    : pin1(&pin1)
    , pin2(&pin2)
{
    enabled = false;
}

bool HBridge::begin()
{
    if (!pwm_is_ready_dt(pin1)) {
        printf("pin1 PWM device not ready\n");
        return false;
    }
    if (!pwm_is_ready_dt(pin2)) {
        printf("pin2 PWM device not ready\n");
        return false;
    }

    if (pwm_set_pulse_dt(pin1, 0)) {
        printf("Failed to set pin1 PWM pulse\n");
        return false;
    }
    if (pwm_set_pulse_dt(pin2, 0)) {
        printf("Failed to set pin2 PWM pulse\n");
        return false;
    }
    return true;
}
void HBridge::setSpeed(float speed)
{
    if (!enabled) {
        return;
    }
    if (speed > 1) {
        speed = 1;
    }
    if (speed < -1) {
        speed = -1;
    }
    if (speed > 0) {
        pwm_set_pulse_dt(pin1, speed * pin1->period);
        pwm_set_pulse_dt(pin2, 0);
    } else if (speed < 0) {
        pwm_set_pulse_dt(pin1, 0);
        pwm_set_pulse_dt(pin2, -speed * pin2->period);
    } else {
        pwm_set_pulse_dt(pin1, 0);
        pwm_set_pulse_dt(pin2, 0);
    }
}
void HBridge::setEnable(bool enable)
{
    if (enable == false) {
        pwm_set_pulse_dt(pin1, 0);
        pwm_set_pulse_dt(pin2, 0);
    }
    enabled = enable;
}
void HBridge::enable()
{
    setEnable(true);
}
void HBridge::disable()
{
    setEnable(false);
}
