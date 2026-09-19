#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include "../include/motor_simulator_pkg/PID.hpp"
#include "../include/motor_simulator_pkg/csv_reader.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

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
        , torque_feedforward_(0.0)
        , angle_controller_(1.0, 0.1, 0.01, -100.0, 100.0, 10.0, PID_MODE_ANGLE)
        , speed_controller_(1.0, 0.1, 0.01, -200.0, 200.0, 10.0, PID_MODE_NORMAL)
        , kFeedforwardB(0.1)
        , kFeedforwardLoadTorque(1.0) {
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

        // Get the path to the CSV file
        std::string package_share_directory =
            ament_index_cpp::get_package_share_directory("motor_simulator_pkg");
        std::string csv_file_path = package_share_directory + "/data/pid_tuning.csv";

        // 从CSV文件读取PID参数
        CsvPidReader pid_reader(csv_file_path);
        PidTuningPoint angle_pid_point, speed_pid_point;

        if (pid_reader.find("angle_controller", angle_pid_point)) {
            angle_controller_ =
                PID(angle_pid_point.Kp, angle_pid_point.Ki, angle_pid_point.Kd, -100.0, 100.0, 10.0,
                    PID_MODE_ANGLE);
        }

        if (pid_reader.find("speed_controller", speed_pid_point)) {
            speed_controller_ =
                PID(speed_pid_point.Kp, speed_pid_point.Ki, speed_pid_point.Kd, -200.0, 200.0, 10.0,
                    PID_MODE_NORMAL);
        }

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
        float speed_cmd = angle_controller_.PID_Calculate(target_angle_, actual_angle_, 0.001);

        // 前馈：抵消模型中的粘性阻力 B*speed 与负载力矩 Load_Torque（模型逆运算 tau = B*omega + Load_Torque）
        torque_feedforward_ = kFeedforwardB * speed_cmd + kFeedforwardLoadTorque;

        // 使用速度PID控制器计算所需的扭矩
        torque_ = speed_controller_.PID_Calculate(speed_cmd, actual_speed_, 0.001)+ torque_feedforward_;
        
        //增加前馈后输出限幅
        if (torque_ > 200.0) {
            torque_ = 200.0;
        } else if (torque_ < -200.0) {
            torque_ = -200.0;
        }

        // 发布扭矩命令
        auto torque_msg = std_msgs::msg::Float64();
        torque_msg.data = torque_;
        torque_publisher_->publish(torque_msg);

        // 发布速度命令
        auto speed_msg = std_msgs::msg::Float64();
        speed_msg.data = speed_cmd;
        speed_cmd_publisher_->publish(speed_msg);
    }

    float target_angle_;
    float actual_angle_;

    float target_speed_;
    float actual_speed_;

    float torque_;
    float torque_feedforward_;

    PID angle_controller_;
    PID speed_controller_;

    float kFeedforwardB;          // 粘性阻尼系数，对应 MotorModel 的 B_
    float kFeedforwardLoadTorque; // 负载力矩，对应 MotorModel 的 Load_Torque_

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