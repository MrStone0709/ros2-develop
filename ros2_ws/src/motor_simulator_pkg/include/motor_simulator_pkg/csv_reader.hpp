#ifndef MOTOR_SIMULATOR_PKG__CSV_READER_HPP_
#define MOTOR_SIMULATOR_PKG__CSV_READER_HPP_

#include <string>
#include <vector>

struct CsvDataPoint {
    double time;
    double value;
};

class CsvReader {
public:
    explicit CsvReader(const std::string& filename);

    const std::vector<CsvDataPoint>& getData() const;

private:
    std::vector<CsvDataPoint> data_;
};

struct PidTuningPoint {
    std::string controller;
    float Kp;
    float Ki;
    float Kd;
};

class CsvPidReader {
public:
    explicit CsvPidReader(const std::string& filename);

    const std::vector<PidTuningPoint>& getData() const;

    // 按 controller 名字查找，未找到返回 false
    bool find(const std::string& controller, PidTuningPoint& point) const;

private:
    std::vector<PidTuningPoint> data_;
};

#endif