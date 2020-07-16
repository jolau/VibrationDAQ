/* Copyright (c) 2020, Jonas Lauener & Wingtra AG
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <string>
#include <vibration_daq/entities/RecordingConfig.hpp>
#include <vibration_daq/entities/RecordingMode.hpp>
#include "yaml-cpp/yaml.h"
#include "vibration_daq/entities/VibrationSensorConfig.hpp"

namespace vibration_daq {
    /**
     * The ConfigModule parses the config.yaml files.
     */
    class ConfigModule {
    private:
        YAML::Node configNode;

        template<typename T>
        static bool convertNode(const YAML::Node &node, T &rhs);

        static bool readVibrationSensor(const YAML::Node &node, VibrationSensorConfig &vibrationSensor);

        static bool readRecordingConfig(const YAML::Node &node, RecordingConfig &recordingConfig);

        static bool readMFFTConfig(const YAML::Node &node, MFFTConfig &mfftConfig);

        static bool readMTCConfig(const YAML::Node &node, MTCConfig &mtcConfig);

    public:
        /**
         * Setups module, checks if it's yaml file.
         * @param configFile
         * @return true == successful setup
         */
        bool setup(std::string configFile);

        /**
         * @return true if read-out is successful
         */
        bool readVibrationSensors(std::vector<VibrationSensorConfig> &vibrationSensors);

        /**
         * @return true if read-out is successful
         */
        bool readRecordingsCount(int &recordingsCount);

        /**
         * @return true if read-out is successful
         */
        bool readStorageDirectoryPath(std::string &storageDirectory) const;

        /**
         * @return true if read-out is successful
         */
        bool readExternalTriggerConfig(bool &externalTriggerActivated, int &externalTriggerPin) const;

        /**
         * @return true if read-out is successful
         */
        bool readStatusLedConfig(bool &statusLedActivated, int &statusLedPin) const;
    };
}
