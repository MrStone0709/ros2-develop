#ifndef MOTOR_SIM_PKG__MOTOR_MODEL_HPP_
#define MOTOR_SIM_PKG__MOTOR_MODEL_HPP_

class MotorModel {
public:
    MotorModel(double J, double B, double Load_Torque);

    void update(double torque, double dt);

    double getSpeed() const;
    double getAngle() const;

private:
    double J_;           // 转动惯量include
    double B_;           // 粘性阻尼系数
    double Load_Torque_; // 负载力矩

    double speed_;
    double angle_;
};

#endif  