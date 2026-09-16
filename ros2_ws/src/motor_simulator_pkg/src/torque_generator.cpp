#include <chrono>
#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

class TorqueGeneratorNode : public rclcpp::Node {
public:
    TorqueGeneratorNode()
        : Node("torque_generator_node")
        , time_(0.0)
        , last_torque_(0) {
        torque_publisher_ = this->create_publisher<std_msgs::msg::Float64>("/torque_cmd", 10);
        timer_ = this->create_wall_timer(1ms, std::bind(&TorqueGeneratorNode::timerCallback, this));
        RCLCPP_INFO(this->get_logger(), "Torque generator started.");
    }

private:
    void timerCallback() {
        constexpr double kDt = 0.001;    // 1 ms simulation step
        constexpr int kTorqueLevels = 10;

        // Hold every level for 1 s, then advance: 1 -> 2 -> ... -> 10 -> 1 -> ...
        const int torque = static_cast<int>(std::floor(time_)) % kTorqueLevels + 1;

        std_msgs::msg::Float64 msg;
        msg.data = static_cast<double>(torque);
        torque_publisher_->publish(msg);

        // Log only when the level changes, so it is obvious which build is running.
        if (torque != last_torque_) {
            last_torque_ = torque;
            RCLCPP_INFO(this->get_logger(), "Published torque_cmd = %d", torque);
        }

        time_ += kDt;
    }

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr torque_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    double time_;
    int last_torque_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TorqueGeneratorNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}