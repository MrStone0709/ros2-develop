#include "../include/motor_simulator_pkg/csv_reader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

// CsvReader 实现
CsvReader::CsvReader(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open CSV file: " + filename);
    }

    std::string line;

    // 跳过 CSV 第一行表头
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);

        std::string time_str;
        std::string value_str;

        if (!std::getline(ss, time_str, ',')) {
            continue;
        }

        if (!std::getline(ss, value_str, ',')) {
            continue;
        }

        CsvDataPoint point;

        point.time = std::stod(time_str);
        point.value = std::stod(value_str);

        data_.push_back(point);
    }

    if (data_.empty()) {
        throw std::runtime_error("CSV file contains no valid data: " + filename);
    }
}

const std::vector<CsvDataPoint>& CsvReader::getData() const { return data_; }

// CsvPidReader 实现
CsvPidReader::CsvPidReader(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open CSV file: " + filename);
    }

    std::string line;

    // 跳过 CSV 第一行表头
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);

        std::string controller_str;
        std::string kp_str;
        std::string ki_str;
        std::string kd_str;

        if (!std::getline(ss, controller_str, ',')) {
            continue;
        }

        if (!std::getline(ss, kp_str, ',')) {
            continue;
        }

        if (!std::getline(ss, ki_str, ',')) {
            continue;
        }

        if (!std::getline(ss, kd_str)) {
            continue;
        }

        PidTuningPoint point;

        point.controller = controller_str;
        point.Kp = std::stof(kp_str);
        point.Ki = std::stof(ki_str);
        point.Kd = std::stof(kd_str);

        data_.push_back(point);
    }

    if (data_.empty()) {
        throw std::runtime_error("CSV file contains no valid data: " + filename);
    }
}

const std::vector<PidTuningPoint>& CsvPidReader::getData() const { return data_; }

bool CsvPidReader::find(const std::string& controller, PidTuningPoint& point) const {
    for (const auto& data_point : data_) {
        if (data_point.controller == controller) {
            point = data_point;
            return true;
        }
    }

    return false;
}