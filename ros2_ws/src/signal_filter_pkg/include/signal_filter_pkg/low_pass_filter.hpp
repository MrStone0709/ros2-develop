#ifndef SIGNAL_FILTER_PKG__LOW_PASS_FILTER_HPP_
#define SIGNAL_FILTER_PKG__LOW_PASS_FILTER_HPP_

class LowPassFilter {
public:
    explicit LowPassFilter(double alpha);

    double filter(double input);

    void reset(double value = 0.0);

private:
    double alpha_;
    double previous_output_;
    bool initialized_;
};

#endif // SIGNAL_FILTER_PKG__LOW_PASS_FILTER_HPP_