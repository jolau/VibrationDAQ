//
// Created by Jonas Lauener on 18.03.20.
//

#include <iostream>
#include "vibration_daq/ConfigModule.hpp"
#include "loguru/loguru.hpp"

namespace vibration_daq {
    template<typename T>
    bool ConfigModule::convertNode(const YAML::Node &node, T &rhs) {
        return YAML::convert<T>::decode(node, rhs);
    }

    bool ConfigModule::readVibrationSensor(const YAML::Node &node, VibrationSensorConfig &vibrationSensor) {
        if (!node.IsMap()) {
            LOG_S(WARNING) << "sensor node is not a map";
            return false;
        }

        if (!convertNode(node["name"], vibrationSensor.name)) {
            LOG_S(WARNING) << "could not read name from config";
            return false;
        }

        if (!convertNode(node["busy_pin"], vibrationSensor.busyPin)) {
            LOG_S(WARNING) << "could not read busy_pin from config";
            return false;
        }

        if (!convertNode(node["reset_pin"], vibrationSensor.resetPin)) {
            LOG_S(WARNING) << "could not read reset_pin from config";
            return false;
        }

        if (!convertNode(node["spi_path"], vibrationSensor.spiPath)) {
            LOG_S(WARNING) << "could not read spi_path from config";
            return false;
        }

        std::string recordingModeString;
        if (!convertNode(node["recording_mode"], recordingModeString)) {
            LOG_S(WARNING) << "could not read decimation_factor from config";
            return false;
        }
        if (!Enum::convert(recordingModeString, vibrationSensor.recordingMode)) {
            LOG_S(WARNING) << "could not convert decimation_factor to enum: " << recordingModeString;
            return false;
        }

        switch (vibrationSensor.recordingMode) {
            case RecordingMode::MFFT:
                if (!readMFFTConfig(node["MFFT_config"], vibrationSensor.mfftConfig)) {
                    LOG_S(WARNING) << "could not read MFFT_config from config";
                    return false;
                }
                return true;
            case RecordingMode::MTC:
                if (!readMTCConfig(node["MTC_config"], vibrationSensor.mtcConfig)) {
                    LOG_S(WARNING) << "could not read MTC_config from config";
                    return false;
                }
                return true;
            case RecordingMode::AFFT:
            case RecordingMode::RTS:
            default:
                LOG_S(WARNING) << "only MFFT and MTC supported.";
                return false;
        }
    }

    bool ConfigModule::readRecordingConfig(const YAML::Node &node, RecordingConfig &recordingConfig) {
        if (!node.IsMap()) {
            LOG_S(WARNING) << "recording node is not a map";
            return false;
        }

        std::string decimationFactorString;
        if (!convertNode(node["decimation_factor"], decimationFactorString)) {
            LOG_S(WARNING) << "could not read decimation_factor from config";
            return false;
        }
        if (!Enum::convert(decimationFactorString, recordingConfig.decimationFactor)) {
            LOG_S(WARNING) << "could not convert decimation_factor to enum: " << decimationFactorString;
            return false;
        }

        std::string firFilterString;
        if (!convertNode(node["fir_filter"], firFilterString)) {
            LOG_S(WARNING) << "could not read fir_filter from config";
            return false;
        }
        if (!Enum::convert(firFilterString, recordingConfig.firFilter)) {
            LOG_S(WARNING) << "could not convert fir_filter to enum: " << firFilterString;
            return false;
        }

        if (recordingConfig.firFilter == FIRFilter::CUSTOM) {
            if (!convertNode(node["custom_filter_taps"], recordingConfig.customFilterTaps)) {
                LOG_S(WARNING) << "could not read custom_filter_taps from config";
                return false;
            }
        }

        return true;
    }

    bool ConfigModule::readMFFTConfig(const YAML::Node &node, MFFTConfig &mfftConfig) {
        if (!node.IsMap()) {
            LOG_S(WARNING) << "mfft node is not a map";
            return false;
        }

        if (!readRecordingConfig(node, mfftConfig)) {
            return false;
        }

        if (!convertNode(node["spectral_avg_count"], mfftConfig.spectralAvgCount)) {
            LOG_S(WARNING) << "could not read spectral_avg_count from config";
            return false;
        }
        if (mfftConfig.spectralAvgCount < 1 || mfftConfig.spectralAvgCount > 255) {
            LOG_S(WARNING) << "spectral_avg_count is not in range (1-255): " << mfftConfig.spectralAvgCount;
            return false;
        }

        std::string windowSettingString;
        if (!convertNode(node["window_setting"], windowSettingString)) {
            LOG_S(WARNING) << "could not read window_setting from config";
            return false;
        }
        if (!Enum::convert(windowSettingString, mfftConfig.windowSetting)) {
            LOG_S(WARNING) << "could not convert window_setting to enum: " << windowSettingString;
            return false;
        }

        return true;
    }

    bool ConfigModule::readMTCConfig(const YAML::Node &node, MTCConfig &mtcConfig) {
        if (!node.IsMap()) {
            LOG_S(WARNING) << "MTC node is not a map";
            return false;
        }

        return readRecordingConfig(node, mtcConfig);
    }

    bool ConfigModule::setup(std::string configFile) {
        configNode = YAML::LoadFile(configFile);
        return configNode.IsMap();
    }

    bool ConfigModule::readVibrationSensors(std::vector<VibrationSensorConfig> &vibrationSensors) {
        auto sensors = configNode["sensors"];
        if (!sensors.IsSequence()) {
            LOG_S(WARNING) << "sensors is not a sequence";
            return false;
        }

        for (const auto &it : sensors) {
            VibrationSensorConfig vibrationSensor;
            if (!readVibrationSensor(it, vibrationSensor)) {
                return false;
            }
            vibrationSensors.push_back(vibrationSensor);
        }

        return true;
    }

    bool ConfigModule::readRecordingsCount(int &recordingsCount) {
        return convertNode(configNode["recordings_count"], recordingsCount);
    }

    bool ConfigModule::readStorageDirectoryPath(std::string &storageDirectory) const {
        return convertNode(configNode["storage_directory"], storageDirectory);
    }

    bool ConfigModule::readExternalTriggerConfig(bool &externalTriggerActivated, int &externalTriggerPin) const {
        if (!convertNode(configNode["external_trigger"], externalTriggerActivated)) {
            LOG_S(WARNING) << "could not read external_trigger from config";
            return false;
        }

        if (externalTriggerActivated) {
            if (!convertNode(configNode["external_trigger_pin"], externalTriggerPin)) {
                LOG_S(WARNING) << "could not read external_trigger_pin from config";
                return false;
            }
        }

        return true;
    }

    bool ConfigModule::readStatusLedConfig(bool &statusLedActivated, int &statusLedPin) const {
        if (!convertNode(configNode["status_led"], statusLedActivated)) {
            LOG_S(WARNING) << "could not read status_led from config";
            return false;
        }

        if (statusLedActivated) {
            if (!convertNode(configNode["status_led_pin"], statusLedPin)) {
                LOG_S(WARNING) << "could not read status_led_pin from config";
                return false;
            }
        }

        return true;
    }
}