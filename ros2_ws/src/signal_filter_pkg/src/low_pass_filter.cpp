#include "../include/signal_filter_pkg/low_pass_filter.hpp"

#include <stdexcept>

LowPassFilter::LowPassFilter(double alpha)
    : alpha_(alpha)
    , previous_output_(0.0)
    , initialized_(false) {
    if (alpha <= 0.0 || alpha > 1.0) {
        throw std::invalid_argument("Alpha must be in the range (0, 1].");
    }
}

double LowPassFilter::filter(double input) {
    // 第一次输入，直接作为初始输出
    if (!initialized_) {
        previous_output_ = input;
        initialized_ = true;

        return previous_output_;
    }

    // 一阶低通滤波
    previous_output_ = alpha_ * input + (1.0 - alpha_) * previous_output_;

    return previous_output_;
}

void LowPassFilter::reset(double value) {
    previous_output_ = value;
    initialized_ = true;
}