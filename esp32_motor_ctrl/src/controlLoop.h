#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H
#include <vector>
class ControlLoop {
public:
    ControlLoop(std::vector<float> _a, std::vector<float> _b);
    ControlLoop(float Kp, float Ki, float Kd, float Ts, float N = 0);
    float calculate(float error);

private:
    std::vector<float> a;
    std::vector<float> b;
    std::vector<float> u;
    std::vector<float> e;
};

#endif // CONTROL_LOOP_H
