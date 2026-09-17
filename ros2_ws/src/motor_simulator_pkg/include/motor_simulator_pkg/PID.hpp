#ifndef MOTOR_SIMULATOR_PKG__PID_HPP_
#define MOTOR_SIMULATOR_PKG__PID_HPP_

#define PID_MODE_NORMAL 0
#define PID_MODE_ANGLE  1
class PID {
public:
    PID(float Kp, float Ki, float Kd, float MinOutput, float MaxOutput, float MaxIntegral,
        char Mode);

    float PID_Calaulate(float Target, float Actual, float dt);

private:
    float Kp;
    float Ki;
    float Kd;

    float Error;
    float LastError;
    float ErrorIntegral;

    float Output;

    float MaxOutput;
    float MinOutput;

    float MaxIntegral;

    char Mode;
};

#endif
