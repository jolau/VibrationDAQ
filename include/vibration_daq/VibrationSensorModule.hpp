//
// Created by Jonas Lauener on 04.03.20.
//
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
    class VibrationSensorModule {
    private:
        const std::string GPIO_PATH = "/dev/gpiochip0";

        gpio_t *gpioBusy, *gpioReset;
        spi_t *spi;
        std::string name;

        RecordingMode currentRecordingMode = RecordingMode::MTC; // default for sensor as well

        static std::vector<float> generateSteps(float stepSize, int samplesCount);

        WordBuffer transfer(WordBuffer sendBuf) const;
        WordBuffer transferWaiting(WordBuffer sendBuf) const;

        uint16_t read(SpiCommand cmd) const;
        std::vector<float> readBuffer(SpiCommand cmd, int samplesCount,
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
        [[nodiscard]] bool setup(unsigned int resetPin, unsigned int busyPin, std::string spiPath, uint32_t speed);
        void close();

        const std::string &getName() const;
        void activateExternalTrigger() const;

        void triggerRecording() const;
        VibrationData retrieveVibrationData() const;
        bool activateMode(const MFFTConfig &mfftConfig);
        bool activateMode(const MTCConfig &mtcConfig);

    };
}
