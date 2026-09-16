#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include "../include/signal_filter_pkg/low_pass_filter.hpp"
#include "../include/signal_filter_pkg/median_filter.hpp"

class SignalFilter : public rclcpp::Node {
public:
    SignalFilter()
        : Node("signal_filter")
        , median_filter_(5)
        , low_pass_filter_(0.2) {
        // 创建订阅者
        subscription_ = this->create_subscription<std_msgs::msg::Float64>(
            "raw_signal", 10,
            [this](const std_msgs::msg::Float64::SharedPtr message)
            {
            this->signal_callback(message);
            }
        );
        // 创建中值滤波后的发布者
        median_publisher_ = this->create_publisher<std_msgs::msg::Float64>("median_signal", 10);

        // 创建低通滤波后的发布者
        lowpass_publisher_ = this->create_publisher<std_msgs::msg::Float64>("lowpass_signal", 10);

        RCLCPP_INFO(this->get_logger(), "Signal filter started.");

        RCLCPP_INFO(this->get_logger(), "Median filter window size: 5");

        RCLCPP_INFO(this->get_logger(), "Low-pass filter alpha: 0.2");
    }

private:
    void signal_callback(const std_msgs::msg::Float64::SharedPtr message) {
        // 获取原始信号
        double input = message->data;

        // 中值滤波
        double median_output = median_filter_.filter(input);

        // 低通滤波
        double lowpass_output = low_pass_filter_.filter(input);

        // 创建中值滤波消息
        std_msgs::msg::Float64 median_message;
        median_message.data = median_output;

        // 创建低通滤波消息
        std_msgs::msg::Float64 lowpass_message;
        lowpass_message.data = lowpass_output;

        // 发布处理后的信号
        median_publisher_->publish(median_message);
        lowpass_publisher_->publish(lowpass_message);
    }

private:
    // ROS 2 通信对象
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscription_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr median_publisher_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr lowpass_publisher_;

    // 滤波器对象
    MedianFilter median_filter_;
    LowPassFilter low_pass_filter_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SignalFilter>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}