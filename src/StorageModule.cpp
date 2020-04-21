//
// Created by Jonas Lauener on 23.03.20.
//

#include <fstream>
#include "vibration_daq/StorageModule.hpp"
#include "loguru/loguru.hpp"

std::string
vibration_daq::StorageModule::getLocalTimestampString(const std::chrono::system_clock::time_point &timePoint) {
    auto localTimePoint = date::make_zoned(date::current_zone(),
                                           date::floor<std::chrono::milliseconds>(timePoint));
    return date::format("%FT%T", localTimePoint);
}

bool vibration_daq::StorageModule::setup(const fs::path& storageDirectoryPath) {
    if (!fs::is_directory(storageDirectoryPath)) {
        LOG_S(ERROR) << "Storage directory does not exist: " << storageDirectoryPath;
        return false;
    }
    this->storageDirectory = storageDirectoryPath;
    return true;
}

bool vibration_daq::StorageModule::storeVibrationData(const vibration_daq::VibrationData &vibrationData,
                                                      const std::string &sensorName,
                                                      const std::chrono::system_clock::time_point &measurementTimestamp) const {
    std::ostringstream dataFilePath;
    dataFilePath << storageDirectory.string();
    dataFilePath << "vibration_data_";
    dataFilePath << Enum::toString(vibrationData.recordingMode);
    dataFilePath << "_";
    dataFilePath << sensorName;
    dataFilePath << "_";
    dataFilePath << getLocalTimestampString(measurementTimestamp);
    dataFilePath << ".csv";

    auto dataFile = std::fstream(dataFilePath.str(), std::ios::out);
    if (!dataFile) {
        LOG_S(ERROR) << "Could not create data file.";
        return false;
    }

    switch (vibrationData.recordingMode) {
        case RecordingMode::MTC:
            dataFile << "Time [s],x-axis [g],y-axis [g],z-axis [g]" << std::endl;
            break;
        case RecordingMode::MFFT:
        case RecordingMode::AFFT:
            dataFile << "Frequency Bin [Hz],x-axis [mg],y-axis [mg],z-axis [mg]" << std::endl;
            break;
        case RecordingMode::RTS:
            LOG_S(ERROR) << "RTS mode not supported!";
            return false;
    }

    for (int i = 0; i < vibrationData.xAxis.size(); ++i) {
        dataFile << vibrationData.stepAxis[i] << "," << vibrationData.xAxis[i] << "," << vibrationData.yAxis[i] << ","
                 << vibrationData.zAxis[i] << std::endl;
    }

    dataFile.close();

    LOG_S(INFO) << "Vibration data stored to file: " << dataFilePath.str();

    return dataFile.good();
}

