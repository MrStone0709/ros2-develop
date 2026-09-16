#include <chrono>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

struct TorquePoint {
    double time;
    double torque;
};

class TorqueGeneratorNode : public rclcpp::Node {
public:
    TorqueGeneratorNode()
        : Node("torque_generator_node")
        , current_index_(0) {
        torque_publisher_ = this->create_publisher<std_msgs::msg::Float64>("/torque_cmd", 10);
        // 读取 CSV 文件
        loadCsv("/workspaces/ros2-develop/ros2_ws/src/motor_simulator_pkg/data/torque_test.csv");
        timer_ = this->create_wall_timer(1ms, std::bind(&TorqueGeneratorNode::timerCallback, this));
        RCLCPP_INFO(this->get_logger(), "Torque generator started.");
    }

private:
    void loadCsv(const std::string& filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open CSV file: %s", filename.c_str());

            throw std::runtime_error("Cannot open CSV file");
        }

        std::string line;

        // 跳过第一行表头
        std::getline(file, line);

        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            std::stringstream ss(line);

            std::string time_str;
            std::string torque_str;

            std::getline(ss, time_str, ',');
            std::getline(ss, torque_str, ',');

            TorquePoint point;

            point.time = std::stod(time_str);
            point.torque = std::stod(torque_str);

            torque_data_.push_back(point);
        }

        file.close();
    }

    void timerCallback() {
        if (current_index_ >= torque_data_.size()) {
            // CSV播放结束
            std_msgs::msg::Float64 msg;
            msg.data = 0.0;

            torque_publisher_->publish(msg);

            return;
        }

        double current_time = current_index_ * 0.001;

        // 到达 CSV 中当前数据点的时间
        if (current_time >= torque_data_[current_index_].time) {
            double torque = torque_data_[current_index_].torque;

            std_msgs::msg::Float64 msg;
            msg.data = torque;

            torque_publisher_->publish(msg);

            //RCLCPP_INFO(
            //    this->get_logger(), "time = %.3f s, torque = %.3f Nm",
            //    torque_data_[current_index_].time, torque);

            current_index_++;
        }
    }

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr torque_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::vector<TorquePoint> torque_data_;
    size_t current_index_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TorqueGeneratorNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}