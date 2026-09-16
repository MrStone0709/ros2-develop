#include "../include/motor_simulator_pkg/motor_model.hpp"

MotorModel::MotorModel(double J, double B, double Load_Torque)
    : J_(J)
    , B_(B)
    , Load_Torque_(Load_Torque)
    , speed_(0.0)
    , angle_(0.0) {}

void MotorModel::update(double torque, double dt) {
    double acceleration = (torque - B_ * speed_ - Load_Torque_) / J_;
    speed_ += acceleration * dt;
    angle_ += speed_ * dt;
}

double MotorModel::getSpeed() const {
    return speed_;
}

double MotorModel::getAngle() const {
    return angle_;
}