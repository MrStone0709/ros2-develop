#include "../include/signal_filter_pkg/median_filter.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

MedianFilter::MedianFilter(std::size_t window_size)
    : window_size_(window_size) {
    // 中值滤波窗口应使用奇数
    if (window_size_ == 0 || window_size_ % 2 == 0) {
        throw std::invalid_argument("Median filter window size must be a positive odd number.");
    }
}

double MedianFilter::filter(double input) {
    // 将新的数据加入窗口
    window_.push_back(input);

    // 如果窗口超过指定大小，删除最旧的数据
    if (window_.size() > window_size_) {
        window_.pop_front();
    }

    // 复制当前窗口
    std::vector<double> sorted_window(window_.begin(), window_.end());

    // 排序
    std::sort(sorted_window.begin(), sorted_window.end());

    // 获取中值
    const std::size_t median_index = sorted_window.size() / 2;

    return sorted_window[median_index];
}

void MedianFilter::reset() { window_.clear(); }