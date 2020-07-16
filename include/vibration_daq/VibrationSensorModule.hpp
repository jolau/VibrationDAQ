/* Copyright (c) 2020, Jonas Lauener & Wingtra AG
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <array>
#include <gpio.h>
#include <spi.h>
#include <vector>
#include <functional>
#include "ADcmXL3021Library.hpp"
#include "utils/HexUtils.hpp"
#include "entities/VibrationData.hpp"
#include "entities/RecordingMode.hpp"
#include "entities/RecordingConfig.hpp"
#include "entities/FIRFilter.hpp"
#include "entities/DecimationFactor.hpp"
#include "entities/WindowSetting.hpp"

namespace vibration_daq {

    /**
     * The VibrationSensorModule handles the whole communication of one sensor over SPI.
     */
    class VibrationSensorModule {
    private:
        const std::string GPIO_PATH = "/dev/gpiochip0";

        gpio_t *gpioBusy, *gpioReset;
        spi_t *spi;
        std::string name;

        RecordingMode currentRecordingMode = RecordingMode::MTC; // default for sensor as well

        static std::vector<float> generateSteps(float stepSize, int samplesCount);

        /**
         * Send 16bit-word over SPI and read response to the sent word.
         * @param sendBuf 16bit-word
         * @return response word
         */
        WordBuffer transfer(WordBuffer sendBuf) const;

        /**
         * Transfers only when sensor is _not_ bus.
         * @param sendBuf 16bit-word
         * @return response word
         */
        WordBuffer transferBlocking(WordBuffer sendBuf) const;

        uint16_t read(SpiCommand cmd) const;
        std::vector<float> readSamplesBuffer(SpiCommand cmd, int samplesCount,
                                             const std::function<float(int16_t)>& convertVibrationValue) const;
        int readRecInfoFFTAveragesCount() const;
        int readRecInfoDecimationFactor() const;

        void write(SpiCommand cmd, uint16_t value) const;
        bool writeRecordingControl(const RecordingMode &recordingMode, const WindowSetting &windowSetting);
        void writeFIRFilter(FIRFilter firFilter);
        void writeCustomFIRFilterTaps(std::array<int16_t, 32> customFilterTaps);
        bool activateMode(const RecordingConfig &recordingConfig, const RecordingMode &recordingMode, const WindowSetting &windowSetting = WindowSetting::HANNING);
    public:
        explicit VibrationSensorModule(const std::string &name);
        /**
         * Setup SPI and GPIO for sensor.
         * @param resetPin BCM pin number
         * @param busyPin BCM pin number
         * @param spiPath full linux path to SPI ex: "/dev/spidev0.0"
         * @param speed in Hz
         * @return true == successful setup, right model (ADcmXL3021) is connected and the connection works
         */
        [[nodiscard]] bool setup(unsigned int resetPin, unsigned int busyPin, std::string spiPath, uint32_t speed);
        void close();

        const std::string &getSensorName() const;

        void activateExternalTrigger() const;
        /**
         * Triggers autonull of sensor and saves offset settings in flash.
         */
        void triggerAutonull() const;
        void triggerRecording() const;
        void restoreFactorySettings();

        /**
         * Retrieve data from sensor, already converted in physical quantities.
         */
        VibrationData retrieveVibrationData() const;

        bool activateMode(const MFFTConfig &mfftConfig);
        bool activateMode(const MTCConfig &mtcConfig);
    };
}
