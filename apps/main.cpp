#include <iostream>
#include <vibration_daq/VibrationSensorModule.hpp>
#include <vibration_daq/utils/HexUtils.hpp>
#include <filesystem>
#include <vibration_daq/ConfigModule.hpp>
#include "loguru/loguru.hpp"
#include <fstream>
#include <date/date.h>
#include <chrono>
#include <date/tz.h>
#include <vibration_daq/StorageModule.hpp>
#include "chrono"
#include "thread"
#include "yaml-cpp/yaml.h"
#include "date/date.h"

using namespace vibration_daq;
using std::cout;
using std::endl;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

static const int SPI_SPEED = 14000000;
gpio_t *gpioTrigger;
std::vector<VibrationSensorModule> vibrationSensorModules;
ConfigModule configModule;
StorageModule storageModule;

//template<typename T>
//bool convertNode(const YAML::Node &node, T &rhs) {
//    return YAML::convert<T>::decode(node, rhs);
//}

bool setupVibrationSensorModules(const bool &externalTriggerActivated);

system_clock::time_point triggerVibrationSensors(const bool &externalTrigger);

int main(int argc, char *argv[]) {
    loguru::g_preamble_uptime = false;
    loguru::g_preamble_thread = false;
    loguru::init(argc, argv);

    std::string configFilePath = "";
    if(argc > 1) {
        configFilePath = argv[1];
    } else {
        LOG_S(ERROR) << "No config file as program argument specified!";
    }
    LOG_S(INFO) << "Config file path: " << configFilePath;

    if (!configModule.setup(configFilePath)) {
        LOG_S(ERROR) << "Could not setup ConfigModule.";
        return EXIT_FAILURE;
    }

    bool externalTriggerActivated = false;
    int externalTriggerPin = -1;
    if (!configModule.readExternalTrigger(externalTriggerActivated, externalTriggerPin)) {
        LOG_S(ERROR) << "Could not retrieve externalTrigger config.";
        return EXIT_FAILURE;
    }
    if (externalTriggerActivated) {
        gpioTrigger = gpio_new();
        if (gpio_open(gpioTrigger, "/dev/gpiochip0", externalTriggerPin, GPIO_DIR_OUT_LOW) < 0) {
            LOG_F(ERROR, "gpio_open(): %s", gpio_errmsg(gpioTrigger));
            return EXIT_FAILURE;
        }
    }

    if (!setupVibrationSensorModules(externalTriggerActivated)) {
        return EXIT_FAILURE;
    }

    std::string storageDirectoryPath;
    if (!configModule.readStorageDirectory(storageDirectoryPath)) {
        LOG_S(ERROR) << "Could not retrieve storage_directory from config.";
        return EXIT_FAILURE;
    }
    if (!storageModule.setup({storageDirectoryPath})) {
        LOG_S(ERROR) << "Could not setup StorageModule.";
        return EXIT_FAILURE;
    }

    int recordingsCount;
    if (!configModule.readRecordingsCount(recordingsCount)) {
        recordingsCount = 1;
    }

    // run indefinitely if recordingsCount == 0
    for (int i = 0; i < recordingsCount || recordingsCount == 0; ++i) {
        system_clock::time_point triggerTime = triggerVibrationSensors(externalTriggerActivated);

        for (const auto &vibrationSensorModule : vibrationSensorModules) {
            auto vibrationData = vibrationSensorModule.retrieveVibrationData();

            bool storedVibrationData = storageModule.storeVibrationData(vibrationData, vibrationSensorModule.getName(),
                                                                        triggerTime);
            LOG_IF_F(ERROR, !storedVibrationData, "Could not store vibration data.");
        }
    }

    for (auto &vibrationSensorModule : vibrationSensorModules) {
        vibrationSensorModule.close();
    }

    return EXIT_SUCCESS;

// TODO: to be deleted
    //    YAML::Node config = YAML::LoadFile("/home/pi/Projects/VibrationDAQ/config.yaml");
//
//    auto sensors = config["sensors0"];
//    cout << sensors.IsMap() << endl;

    /*auto mfftNode = config["MFFT"];
    if (mfftNode) {
        std::cout << "exists: " << convertNode(mfftNode["custom_filter_taps"], spectralAvgCount) << "\n";
        std::cout << "value: " << spectralAvgCount[0] << std::endl;
    }

     const int PIN_BUSY = 22;
     const int PIN_RESET = 27;
     const std::string SPI_PATH = "/dev/spidev0.0";

     vibrationSensorModule.setup(PIN_RESET, PIN_BUSY, 14000000);

 //    cout << "change recording mode worked: " << vibrationSensorModule.writeRecordingControl(RecordingMode::MTC, WindowSetting::RECTANGULAR)
 //         << std::endl;

 //    MFFTConfig mfftConfig;
 //    mfftConfig.spectralAvgCount = 1;
 //    mfftConfig.firFilter = FIRFilter::NO_FILTER;
 //    mfftConfig.decimationFactor = DecimationFactor::FACTOR_1;
 //    mfftConfig.windowSetting = WindowSetting::HANNING;
 //    vibrationSensorModule.activateMode(mfftConfig);

 //    vibrationSensorModule.activateModeMTC(DecimationFactor::FACTOR_1, FIRFilter::NO_FILTER);

     // start recording
     vibrationSensorModule.triggerRecording();

     auto vibrationData = vibrationSensorModule.retrieveVibrationData();

     auto startTime = std::chrono::high_resolution_clock::now();
     auto dataFile = std::fstream("/home/pi/Documents/vibrationData.csv", std::ios::out | std::ios::binary);

     dataFile << "x-Axis,y-Axis,z-Axis" << std::endl;
     for (int i = 0; i < vibrationData.xAxis.size(); ++i) {
         dataFile << vibrationData.xAxis[i] << "," << vibrationData.yAxis[i] << "," << vibrationData.zAxis[i]
                  << std::endl;
     }

 //    dataFile.write((char*)&vibrationData.xAxis[0], vibrationData.xAxis.size() * sizeof(float));

     dataFile.close();
     auto endTime = std::chrono::high_resolution_clock::now();

     cout << "time to save file: "
          << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms" << std::endl;

     vibrationSensorModule.close();

     return 0;*/
}

system_clock::time_point triggerVibrationSensors(const bool &externalTrigger) {
    system_clock::time_point triggerTime;
    if (externalTrigger) {
        if (gpio_write(gpioTrigger, true) < 0) {
            fprintf(stderr, "gpio_write(): %s", gpio_errmsg(gpioTrigger));
            exit(1);
        }

        triggerTime = system_clock::now();
        sleep_for(5ms);

        if (gpio_write(gpioTrigger, false) < 0) {
            fprintf(stderr, "gpio_write(): %s", gpio_errmsg(gpioTrigger));
            exit(1);
        }
    } else {
        for (const auto &vibrationSensorModule : vibrationSensorModules) {
            // start recording
            cout << "triggered" << endl;
            vibrationSensorModule.triggerRecording();
        }
        triggerTime = system_clock::now();
    }

    return triggerTime;
}

bool setupVibrationSensorModules(const bool &externalTriggerActivated) {
    std::vector<VibrationSensorConfig> vibrationSensorConfigs;
    if (!configModule.readVibrationSensors(vibrationSensorConfigs)) {
        LOG_S(ERROR) << "Could not retrieve vibration sensors from config.";
        return false;
    }

    for (const auto &vibrationSensorConfig : vibrationSensorConfigs) {
        VibrationSensorModule vibrationSensorModule(vibrationSensorConfig.name);
        if (!vibrationSensorModule.setup(vibrationSensorConfig.resetPin, vibrationSensorConfig.busyPin,
                                         vibrationSensorConfig.spiPath,
                                         SPI_SPEED)) {
            LOG_S(ERROR) << "Could not setup vibration sensor: " << vibrationSensorConfig.name;
            return false;
        }

        if (externalTriggerActivated) {
            vibrationSensorModule.activateExternalTrigger();
        }

        switch (vibrationSensorConfig.recordingMode) {
            case RecordingMode::MFFT:
                vibrationSensorModule.activateMode(vibrationSensorConfig.mfftConfig);
                break;
            case RecordingMode::MTC:
                vibrationSensorModule.activateMode(vibrationSensorConfig.mtcConfig);
                break;
        }

        vibrationSensorModules.push_back(vibrationSensorModule);
    }

    return true;
}
