#include "controlLoop.h"
#include <vector>
#include <stdio.h>
// difference equation control loop - can run any control loop, not just PID
// References:
//   https://www.scilab.org/discrete-time-pid-controller-implementation
//   Notes from Control Systems class with Professor Ahmed Zaki, Brown University
//   https://www.mathworks.com/help/control/ug/discrete-time-proportional-integral-derivative-pid-controller.html
ControlLoop::ControlLoop(std::vector<float> _a, std::vector<float> _b)
{
    a = _a;
    b = _b;
    u = std::vector<float>(a.size(), 0);
    e = std::vector<float>(b.size(), 0);
}

ControlLoop::ControlLoop(float Kp, float Ki, float Kd, float Ts, float N)
{
    float a0 = (1 + N * Ts);
    float a1 = -(2 + N * Ts);
    float a2 = 1;
    float b0 = Kp * (1 + N * Ts) + Ki * Ts * (1 + N * Ts) + Kd * N;
    float b1 = -(Kp * (2 + N * Ts) + Ki * Ts + 2 * Kd * N);
    float b2 = Kp + Kd * N;

    a = { 0, a1 / a0, a2 / a0 };
    b = { b0 / a0, b1 / a0, b2 / a0 };

    u = std::vector<float>(a.size(), 0);
    e = std::vector<float>(b.size(), 0);
}

/**
 * @brief  this runs a step of the control loop
 * @note   run this at a fixed rate
 * @param  error: error input to the control loop
 * @retval output of the control loop
 */
float ControlLoop::calculate(float error)
{
    for (std::size_t i = e.size() - 1; i > 0; i--) {
        e[i] = e[i - 1];
    }
    e[0] = error;
    for (std::size_t i = u.size() - 1; i > 0; i--) {
        u[i] = u[i - 1];
    }
    u[0] = 0;
    for (std::size_t i = 1; i < a.size(); i++) {
        u[0] += -a[i] * u[i];
    }
    for (std::size_t i = 0; i < b.size(); i++) {
        u[0] += b[i] * e[i];
    }
    return u[0];
}
