#include "../include/motor_simulator_pkg/PID.hpp"

PID::PID(float Kp, float Ki, float Kd, float MinOutput, float MaxOutput, float MaxIntegral, char Mode)
    : Kp(Kp), Ki(Ki), Kd(Kd), MinOutput(MinOutput), MaxOutput(MaxOutput), MaxIntegral(MaxIntegral), Mode(Mode) {
    Error = 0.0f;
    LastError = 0.0f;
    ErrorIntegral = 0.0f;
    Output = 0.0f;
}

float PID::PID_Calaulate(float Target, float Actual, float dt) {

    if(this->Mode == PID_MODE_ANGLE)
    {
        // 角度模式下，计算误差时考虑环绕
        float error = Target - Actual;
        if (error > 180.0f) {
            error -= 360.0f;
        } else if (error < -180.0f) {
            error += 360.0f;
        }
        this->Error = error;
    }
    else
    {
        // 普通模式下，直接计算误差
        this->Error = Target - Actual;
    }
    this->ErrorIntegral+=this->Error*dt;

    /* 积分限幅 */
    if(this->ErrorIntegral > this->MaxIntegral)
    {
        this->ErrorIntegral = this->MaxIntegral;
    }
    else if(this->ErrorIntegral < -this->MaxIntegral)
    {
        this->ErrorIntegral = -this->MaxIntegral;
    }

    this->Output=this->Kp*this->Error+this->Ki*this->ErrorIntegral+this->Kd*(this->Error-this->LastError)/dt;

    /* 输出限幅 */
    if(this->Output > this->MaxOutput)
    {
        this->Output = this->MaxOutput;
    }
    else if(this->Output < this->MinOutput)
    {
        this->Output = this->MinOutput;
    }

    /* 保存本次误差 */
    this->LastError=this->Error;

    return this->Output;
}
