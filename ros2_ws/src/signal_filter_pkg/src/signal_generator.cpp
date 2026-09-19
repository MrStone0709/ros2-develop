#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include <chrono>
#include <cmath>
#include <random>

using namespace std::chrono_literals;

class SignalGenerator : public rclcpp::Node {
public:
    SignalGenerator()
        : Node("signal_generator")
        , random_generator_(std::random_device{}()) {
        // 声明参数
        this->declare_parameter<double>("amplitude", 1.0);
        this->declare_parameter<double>("signal_frequency", 20.0);
        this->declare_parameter<double>("publish_frequency", 2000.0);

        // 获取参数
        amplitude_ = this->get_parameter("amplitude").as_double();
        signal_frequency_ = this->get_parameter("signal_frequency").as_double();
        publish_frequency_ = this->get_parameter("publish_frequency").as_double();

        // 高斯噪声标准差 = 信号幅值的 1%
        noise_stddev_ = std::abs(amplitude_) * 0.01;

        noise_distribution_ = std::normal_distribution<double>(0.0, noise_stddev_);

        // 创建发布者
        publisher_ = this->create_publisher<std_msgs::msg::Float64>("raw_signal", 10);

        // 计算定时器周期
        auto period = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::duration<double>(1.0 / publish_frequency_));

        // 创建定时器
        timer_ = this->create_wall_timer(period, std::bind(&SignalGenerator::publish_signal, this));

        // 记录开始时间
        start_time_ = this->get_clock()->now();

        RCLCPP_INFO(this->get_logger(), "Signal generator started.");

        RCLCPP_INFO(this->get_logger(), "Amplitude: %.3f", amplitude_);

        RCLCPP_INFO(this->get_logger(), "Signal frequency: %.3f Hz", signal_frequency_);

        RCLCPP_INFO(this->get_logger(), "Publish frequency: %.3f Hz", publish_frequency_);

        RCLCPP_INFO(this->get_logger(), "Noise stddev: %.6f", noise_stddev_);
    }

private:
    void publish_signal() {
        // 获取经过的时间
        rclcpp::Duration elapsed = this->get_clock()->now() - start_time_;

        double t = elapsed.seconds();

        // 计算理想正弦信号
        double sine_signal = amplitude_ * std::sin(2.0 * M_PI * signal_frequency_ * t);

        // 生成高斯随机噪声
        double noise = noise_distribution_(random_generator_);

        // 加入噪声
        double signal = sine_signal + noise;

        // 创建 ROS 2 消息
        std_msgs::msg::Float64 message;
        message.data = signal;

        // 发布
        publisher_->publish(message);
    }

private:
    // ROS 2
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    // 信号参数
    double amplitude_;
    double signal_frequency_;
    double publish_frequency_;

    // 噪声参数
    double noise_stddev_;

    // 时间
    rclcpp::Time start_time_;

    // 随机数
    std::mt19937 random_generator_;
    std::normal_distribution<double> noise_distribution_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SignalGenerator>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}