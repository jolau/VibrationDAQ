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
    /**
     * The StorageModule stores the VibrationData on disk.
     */
    class StorageModule {
    private:
        fs::path storageDirectory;

        static std::string getLocalTimestampString(const std::chrono::system_clock::time_point &timePoint);

        static std::string getUTCTimestampString(const std::chrono::system_clock::time_point &timePoint);

    public:
        /**
         * Checks if storage directory is existing.
         * @param storageDirectoryPath
         * @return true if storage directory exists
         */
        bool setup(const fs::path &storageDirectoryPath);

        /**
         * Stores the vibration data as CSV file.
         * @param vibrationData
         * @param sensorName will be used for filename
         * @param measurementTimestamp will be used for filename
         * @return true if success
         */
        bool storeVibrationData(const VibrationData &vibrationData, const std::string &sensorName,
                                const std::chrono::system_clock::time_point &measurementTimestamp) const;
    };
}