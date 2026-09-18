#include "../include/motor_simulator_pkg/csv_reader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

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