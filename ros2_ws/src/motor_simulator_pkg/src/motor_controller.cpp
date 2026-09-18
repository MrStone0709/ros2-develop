#include "../include/motor_simulator_pkg/PID.hpp"
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

class motor_controller : public rclcpp::Node {
public:
    motor_controller()
        : Node("motor_controller")
        , target_angle_(0.0)
        , actual_angle_(0.0)
        , target_speed_(0.0)
        , actual_speed_(0.0)
        , torque_(0.0)
        , angle_controller_(1.0, 0.1, 0.01, 0, 360.0, 5.0, PID_MODE_ANGLE)
        , speed_controller_(1.0, 0.1, 0.01, -100.0, 100.0, 5.0, PID_MODE_NORMAL) {
        // 创建订阅器和发布器
        angle_cmd_subscriber_ = this->create_subscription<std_msgs::msg::Float64>(
            "/angle_cmd", 10,
            std::bind(&motor_controller::angle_cmd_callback, this, std::placeholders::_1));

        motor_angle_subscriber_ = this->create_subscription<std_msgs::msg::Float64>(
            "/motor_angle", 10,
            std::bind(&motor_controller::motor_angle_callback, this, std::placeholders::_1));

        motor_speed_subscriber_ = this->create_subscription<std_msgs::msg::Float64>(
            "/motor_speed", 10,
            std::bind(&motor_controller::motor_speed_callback, this, std::placeholders::_1));

        torque_publisher_ = this->create_publisher<std_msgs::msg::Float64>("/torque_cmd", 10);

        speed_cmd_publisher_ = this->create_publisher<std_msgs::msg::Float64>("/speed_cmd", 10);

        // 创建定时器
        timer_ = this->create_wall_timer(1ms, std::bind(&motor_controller::controlCallback, this));

        RCLCPP_INFO(this->get_logger(), "Motor controller started.");
    }

private:
    void angle_cmd_callback(const std_msgs::msg::Float64::SharedPtr msg) {
        target_angle_ = msg->data;
    }
    void motor_angle_callback(const std_msgs::msg::Float64::SharedPtr msg) {
        actual_angle_ = msg->data;
    }
    void motor_speed_callback(const std_msgs::msg::Float64::SharedPtr msg) {
        actual_speed_ = msg->data;
    }

    void controlCallback() {
        // 计算角度误差并使用PID控制器计算所需的速度
        double speed_cmd = angle_controller_.PID_Calculate(target_angle_, actual_angle_, 0.001);

        // 使用速度PID控制器计算所需的扭矩
        torque_ = speed_controller_.PID_Calculate(speed_cmd, actual_speed_, 0.001);

        // 发布扭矩命令
        auto torque_msg = std_msgs::msg::Float64();
        torque_msg.data = torque_;
        torque_publisher_->publish(torque_msg);

        // 发布速度命令
        auto speed_msg = std_msgs::msg::Float64();
        speed_msg.data = speed_cmd;
        speed_cmd_publisher_->publish(speed_msg);
    }

    double target_angle_;
    double actual_angle_;

    double target_speed_;
    double actual_speed_;

    double torque_;

    PID angle_controller_;
    PID speed_controller_;

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr angle_cmd_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr motor_angle_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr motor_speed_subscriber_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr torque_publisher_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr speed_cmd_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<motor_controller>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}