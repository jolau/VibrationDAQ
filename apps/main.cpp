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
#include "pwm.h"

using namespace vibration_daq;
using std::cout;
using std::endl;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

static const int SPI_SPEED = 14000000;
gpio_t *gpioTrigger;
gpio_t *gpioStatusLed;

std::vector<VibrationSensorModule> vibrationSensorModules;
ConfigModule configModule;
StorageModule storageModule;

bool setupVibrationSensorModules(const bool &externalTriggerActivated);

system_clock::time_point triggerVibrationSensors(const bool &externalTrigger);

int main(int argc, char *argv[]) {
    loguru::g_preamble_uptime = false;
    loguru::g_preamble_thread = false;
    loguru::init(argc, argv);

    std::string configFilePath = "";
    if (argc > 1) {
        configFilePath = argv[1];
    } else {
        LOG_S(ERROR) << "No config file as program argument specified!";
    }
    LOG_S(INFO) << "Loading from config file path: " << configFilePath;

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

    bool statusLedActivated = false;
    int statusLedPin = -1;
    if (!configModule.readStatusLed(statusLedActivated, statusLedPin)) {
        LOG_S(ERROR) << "Could not retrieve externalTrigger config.";
        return EXIT_FAILURE;
    }
    if (statusLedActivated) {
        gpioStatusLed = gpio_new();
        if (gpio_open(gpioStatusLed, "/dev/gpiochip0", statusLedPin, GPIO_DIR_OUT_LOW) < 0) {
            LOG_F(ERROR, "gpio_open(): %s", gpio_errmsg(gpioStatusLed));
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

    if (statusLedActivated && gpio_write(gpioStatusLed, true) < 0) {
        fprintf(stderr, "gpio_write(): %s", gpio_errmsg(gpioStatusLed));
        exit(1);
    }

    // run indefinitely if recordingsCount == 0
    for (int i = 0; i < recordingsCount || recordingsCount == 0; ++i) {
        system_clock::time_point triggerTime = triggerVibrationSensors(externalTriggerActivated);

        for (const auto &vibrationSensorModule : vibrationSensorModules) {
            auto vibrationData = vibrationSensorModule.retrieveVibrationData();

            if (statusLedActivated && gpio_write(gpioStatusLed, false) < 0) {
                fprintf(stderr, "gpio_write(): %s", gpio_errmsg(gpioStatusLed));
                exit(1);
            }

            bool storedVibrationData = storageModule.storeVibrationData(vibrationData, vibrationSensorModule.getName(),
                                                                        triggerTime);

            if (statusLedActivated && gpio_write(gpioStatusLed, true) < 0) {
                fprintf(stderr, "gpio_write(): %s", gpio_errmsg(gpioStatusLed));
                exit(1);
            }

            LOG_IF_F(ERROR, !storedVibrationData, "Could not store vibration data.");
        }


    }

    for (auto &vibrationSensorModule : vibrationSensorModules) {
        vibrationSensorModule.close();
    }

    if (externalTriggerActivated) {
        gpio_close(gpioTrigger);
        gpio_free(gpioTrigger);
    }

    if (statusLedActivated) {
        if (gpio_write(gpioStatusLed, false) < 0) {
            fprintf(stderr, "gpio_write(): %s", gpio_errmsg(gpioStatusLed));
            exit(1);
        }

        gpio_close(gpioStatusLed);
        gpio_free(gpioStatusLed);
    }

    return EXIT_SUCCESS;
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

//        vibrationSensorModule.triggerAutonull();
//        vibrationSensorModule.restoreFactorySettings();

        switch (vibrationSensorConfig.recordingMode) {
            case RecordingMode::MFFT:
                vibrationSensorModule.activateMode(vibrationSensorConfig.mfftConfig);
                break;
            case RecordingMode::MTC:
                vibrationSensorModule.activateMode(vibrationSensorConfig.mtcConfig);
                break;
        }

        vibrationSensorModules.push_back(vibrationSensorModule);
        LOG_S(INFO) << vibrationSensorModule.getName() << " setup done";
    }

    return true;
}
