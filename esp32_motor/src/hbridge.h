#ifndef HBRIDGE_H
#define HBRIDGE_H
#include <zephyr/drivers/pwm.h>

class HBridge {
protected:
    bool enabled;
    const struct pwm_dt_spec* pin1;
    const struct pwm_dt_spec* pin2;

public:
    HBridge(const struct pwm_dt_spec& pin1, const struct pwm_dt_spec& pin2);
    bool begin();
    void setEnable(bool enable);
    void enable();
    void disable();
    void setSpeed(float speed);
};

#endif // HBRIDGE_H
