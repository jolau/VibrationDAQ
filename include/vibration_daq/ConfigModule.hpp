//
// Created by Jonas Lauener on 18.03.20.
//

#pragma once

#include <string>
#include <vibration_daq/entities/RecordingConfig.hpp>
#include <vibration_daq/entities/RecordingMode.hpp>
#include "yaml-cpp/yaml.h"
#include "vibration_daq/entities/VibrationSensorConfig.hpp"

namespace vibration_daq {
    class ConfigModule {
    private:
        YAML::Node configNode;

        template<typename T>
        static bool convertNode(const YAML::Node& node, T &rhs);
        static bool readVibrationSensor(const YAML::Node &node, VibrationSensorConfig &vibrationSensor);
        static bool readRecordingConfig(const YAML::Node &node, RecordingConfig &recordingConfig);
        static bool readMFFTConfig(const YAML::Node &node, MFFTConfig &mfftConfig);
        static bool readMTCConfig(const YAML::Node &node, MTCConfig &mtcConfig);
    public:
        bool setup(std::string configFile);
        bool readVibrationSensors(std::vector<VibrationSensorConfig> &vibrationSensors);
        bool readRecordingsCount(int &recordingsCount);
        bool readStorageDirectory(std::string &storageDirectory) const;
        bool readExternalTrigger(bool &externalTriggerActivated, int &externalTriggerPin) const;
    };
}
