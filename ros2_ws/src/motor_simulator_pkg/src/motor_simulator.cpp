#include "../include/motor_simulator_pkg/motor_model.hpp"

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include <chrono>
#include <memory>

using namespace std::chrono_literals;

class MotorSimulatorNode : public rclcpp::Node {
public:
    MotorSimulatorNode()
        : Node("motor_simulator_node")
        , motor_(0.01, 0.1, 0.0) {
        torque_subscriber_ = this->create_subscription<std_msgs::msg::Float64>(
            "/torque_cmd", 10,
            std::bind(&MotorSimulatorNode::torqueCallback, this, std::placeholders::_1));

        speed_publisher_ = this->create_publisher<std_msgs::msg::Float64>("/motor_speed", 10);

        angle_publisher_ = this->create_publisher<std_msgs::msg::Float64>("/motor_angle", 10);

        timer_ = this->create_wall_timer(1ms, std::bind(&MotorSimulatorNode::simulationCallback, this));

        RCLCPP_INFO(this->get_logger(), "Motor simulator started.");
    }

private:
    void torqueCallback(const std_msgs::msg::Float64::SharedPtr msg) {
        torque_ = msg->data;
    }

    void simulationCallback() {
        double dt = 0.001; // 1 ms time step
        motor_.update(torque_, dt);

        double speed = motor_.getSpeed();
        double angle = motor_.getAngle();
        
        if(angle>360){
            angle = angle - 360;
        }

        std_msgs::msg::Float64 speed_msg;
        std_msgs::msg::Float64 angle_msg;

        speed_msg.data = speed;
        angle_msg.data = angle;

        speed_publisher_->publish(speed_msg);
        angle_publisher_->publish(angle_msg);
    }

    MotorModel motor_;
    double torque_ = 0.0;

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr
        torque_subscriber_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr
        speed_publisher_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr
        angle_publisher_;

    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MotorSimulatorNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
