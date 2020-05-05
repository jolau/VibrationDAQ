//
// Created by Jonas Lauener on 23.03.20.
//
#pragma once

#include <filesystem>
#include <vibration_daq/entities/VibrationData.hpp>
#include <date/tz.h>
#include <chrono>

namespace fs = std::filesystem;

namespace vibration_daq {
    class StorageModule {
    private:
        fs::path storageDirectory;

        static std::string getLocalTimestampString(const std::chrono::system_clock::time_point &timePoint);

        static std::string getTimestampString(const std::chrono::system_clock::time_point &timePoint);

    public:
        bool setup(const fs::path &storageDirectoryPath);

        bool storeVibrationData(const VibrationData &vibrationData, const std::string &sensorName,
                                const std::chrono::system_clock::time_point &measurementTimestamp) const;
    };
}