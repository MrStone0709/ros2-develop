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

#endif