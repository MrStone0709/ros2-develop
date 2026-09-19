#include <chrono>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include "../include/motor_simulator_pkg/csv_reader.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

using namespace std::chrono_literals;

class AngleGenerator : public rclcpp::Node {
public:
    AngleGenerator()
        : Node("angle_generator")
        , current_index_(0) {
        angle_publisher_ = this->create_publisher<std_msgs::msg::Float64>("/angle_cmd", 10);

        // 获取包的共享目录
        std::string package_share_directory =
            ament_index_cpp::get_package_share_directory("motor_simulator_pkg");
        std::string csv_file_path = package_share_directory + "/data/angle_test.csv";

        // 读取 CSV 文件
        CsvReader csv_reader(csv_file_path);
        angle_data_ = csv_reader.getData();

        timer_ = this->create_wall_timer(1ms, std::bind(&AngleGenerator::timerCallback, this));
        RCLCPP_INFO(this->get_logger(), "Angle generator started.");
    }

private:
    void timerCallback() {
        if (current_index_ >= angle_data_.size()) {
            // CSV播放结束
            std_msgs::msg::Float64 msg;
            msg.data = 0.0;

            angle_publisher_->publish(msg);

            return;
        }

        double current_time = current_index_ * 0.001;

        // 到达 CSV 中当前数据点的时间
        if (current_time >= angle_data_[current_index_].time) {
            double angle = angle_data_[current_index_].value;

            std_msgs::msg::Float64 msg;
            msg.data = angle;

            angle_publisher_->publish(msg);

            // RCLCPP_INFO(
            //     this->get_logger(), "time = %.3f s, angle = %.3f rad",
            //     angle_data_[current_index_].time, angle);

            current_index_++;
        }
    }

    std::vector<CsvDataPoint> angle_data_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr angle_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    size_t current_index_;
};

int  main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<AngleGenerator>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}