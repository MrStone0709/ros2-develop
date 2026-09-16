#ifndef SIGNAL_FILTER_PKG__MEDIAN_FILTER_HPP_
#define SIGNAL_FILTER_PKG__MEDIAN_FILTER_HPP_

#include <cstddef>
#include <deque>

class MedianFilter {
public:
    explicit MedianFilter(std::size_t window_size = 5);

    double filter(double input);

    void reset();

private:
    std::size_t window_size_;
    std::deque<double> window_;
};

#endif // SIGNAL_FILTER_PKG__MEDIAN_FILTER_HPP_