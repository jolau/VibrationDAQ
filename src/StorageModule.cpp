/* Copyright (c) 2020, Jonas Lauener & Wingtra AG
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <fstream>
#include "vibration_daq/StorageModule.hpp"
#include "loguru/loguru.hpp"

namespace vibration_daq {
    std::string
    StorageModule::getLocalTimestampString(const std::chrono::system_clock::time_point &timePoint) {
        auto localTimePoint = date::make_zoned(date::current_zone(),
                                               date::floor<std::chrono::milliseconds>(timePoint));

        return date::format("%FT%H_%M_%S", localTimePoint);
    }

    std::string
    StorageModule::getUTCTimestampString(const std::chrono::system_clock::time_point &timePoint) {
        return date::format("%FT%H_%M_%S", date::floor<std::chrono::milliseconds>(timePoint));
    }

    bool StorageModule::setup(const fs::path &storageDirectoryPath) {
        if (!fs::is_directory(storageDirectoryPath)) {
            LOG_S(ERROR) << "Storage directory does not exist: " << storageDirectoryPath;
            return false;
        }
        this->storageDirectory = storageDirectoryPath;
        return true;
    }

    bool StorageModule::storeVibrationData(const vibration_daq::VibrationData &vibrationData,
                                           const std::string &sensorName,
                                           const std::chrono::system_clock::time_point &measurementTimestamp) const {
        std::ostringstream dataFilePath;
        dataFilePath << storageDirectory.string();
        dataFilePath << "vibration_data_";
        dataFilePath << Enum::toString(vibrationData.recordingMode);
        dataFilePath << "_";
        dataFilePath << getUTCTimestampString(measurementTimestamp);
        dataFilePath << "_";
        dataFilePath << sensorName;
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
            dataFile << vibrationData.stepAxis[i] << "," << vibrationData.xAxis[i] << "," << vibrationData.yAxis[i]
                     << ","
                     << vibrationData.zAxis[i] << std::endl;
        }

        dataFile.close();

        LOG_S(INFO) << "Vibration data stored to file: " << dataFilePath.str();

        return dataFile.good();
    }
}
