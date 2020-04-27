//
// Created by Jonas Lauener on 04.03.20.
//

#include <vibration_daq/VibrationSensorModule.hpp>
#include "vibration_daq/utils/HexUtils.hpp"
#include <cmath>
#include <functional>
#include "chrono"
#include "thread"
#include "loguru/loguru.hpp"

namespace vibration_daq {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono_literals;

    VibrationSensorModule::VibrationSensorModule(const std::string &name) : name(name) {}

    const std::string &VibrationSensorModule::getName() const {
        return name;
    }

    WordBuffer VibrationSensorModule::transfer(WordBuffer sendBuf) const {
        WordBuffer recBuf = {};
        if (spi_transfer(spi, sendBuf.data(), recBuf.data(), 2) < 0) {
            LOG_F(ERROR, "spi_transfer(): %s\n", spi_errmsg(spi));
            exit(1);
        }

        // keep processor busy to increase time of SPI disable to ~40us
        for (int i = 0; i < 2000; ++i) {
            // fixes the bug of shifting MISO 1 byte
        }

        return recBuf;
    }

    WordBuffer VibrationSensorModule::transferWaiting(WordBuffer sendBuf) const {
        bool notBusy;

        do {
            if (gpio_read(gpioBusy, &notBusy) < 0) {
                LOG_F(ERROR, "gpio_read(): %s\n", gpio_errmsg(gpioBusy));
                exit(1);
            }

            if (notBusy) {
                return transfer(sendBuf);
            } else {
                DLOG_S(INFO) << name << " is busy.";
                sleep_for(10ms);
            }
        } while (!notBusy);

        return {};
    }

    uint16_t VibrationSensorModule::read(vibration_daq::SpiCommand cmd) const {
        if (!cmd.readFlag) {
            LOG_S(ERROR) << name << ": Cannot read SpiCommand (PageID: " << cmd.pageId << ", Address: " << cmd.address
                         << "). Read flag not set.";
            return 0;
        }

        // select page
        transferWaiting({0x80, cmd.pageId});

        transferWaiting({cmd.address, 0});
        WordBuffer resp = transferWaiting({0, 0});

        return convert(resp);
    }

    void VibrationSensorModule::write(SpiCommand cmd, uint16_t value) const {
        if (!cmd.writeFlag) {
            LOG_S(ERROR) << name << ": Cannot write SpiCommand (PageID: " << cmd.pageId << ", Address: " << cmd.address
                         << "). Write flag not set.";
            return;
        }

        // select page
        transferWaiting({0x80, cmd.pageId});

        transferWaiting({static_cast<unsigned char>(cmd.address | 0x80), static_cast<unsigned char>(value & 0xff)});
        transferWaiting({static_cast<unsigned char>((cmd.address + 1) | 0x80), static_cast<unsigned char>(value >> 8)});
    }


    bool
    VibrationSensorModule::setup(unsigned int resetPin, unsigned int busyPin, std::string spiPath, uint32_t speed) {
        gpioReset = gpio_new();
        gpioBusy = gpio_new();

        if (gpio_open(gpioReset, GPIO_PATH.data(), resetPin, GPIO_DIR_OUT_LOW) < 0) {
            LOG_F(ERROR, "gpio_open(): %s\n", gpio_errmsg(gpioReset));
            return false;
        }
        if (gpio_open(gpioBusy, GPIO_PATH.data(), busyPin, GPIO_DIR_IN) < 0) {
            LOG_F(ERROR, "gpio_open(): %s\n", gpio_errmsg(gpioBusy));
            return false;
        }

        spi = spi_new();
        if (spi_open_advanced(spi, spiPath.data(), 3, speed, spi_bit_order_t::MSB_FIRST, 8, 0) < 0) {
            LOG_F(ERROR, "spi_open(): %s\n", spi_errmsg(spi));
            return false;
        }

        sleep_for(200ms);

        if (gpio_write(gpioReset, true) < 0) {
            LOG_F(ERROR, "gpio_write(): %s\n", gpio_errmsg(gpioReset));
            return false;
        }

        // important for transient behaviour of busy pin on startup!
        sleep_for(500ms);

        // check if the right model (ADcmXL3021) is connected and if the connection works
        uint16_t prodId = read(spi_commands::PROD_ID);
        if (prodId != 0x0BCD) {
            LOG_F(ERROR, "Not getting the right prodId, getting: 0x%04X", prodId);
            return false;
        }

        return true;
    }

    void VibrationSensorModule::close() {
        gpio_close(gpioBusy);
        gpio_close(gpioReset);

        gpio_free(gpioBusy);
        gpio_free(gpioReset);

        spi_close(spi);
        spi_free(spi);
    }

    bool VibrationSensorModule::writeRecordingControl(const RecordingMode &recordingMode,
                                                      const WindowSetting &windowSetting) {
        // hard type sample rate option 0
        uint16_t recCtrl = 0x100;

        recCtrl |= (static_cast<uint8_t>(windowSetting) << 12);
        recCtrl |= static_cast<uint8_t>(recordingMode);

        write(spi_commands::REC_CTRL, recCtrl);

        recCtrl = read(spi_commands::REC_CTRL);
        currentRecordingMode = static_cast<RecordingMode>(recCtrl & 0x3);

        return currentRecordingMode == recordingMode;
    }

    VibrationData VibrationSensorModule::retrieveVibrationData() const {
        int samplesCount = 0;
        float recordStepSize = 0;
        int decimationFactor = readRecInfoDecimationFactor();

        std::function<float(int16_t)> convertVibrationValue;
        switch (currentRecordingMode) {
            case RecordingMode::MTC:
                samplesCount = 4096;
                recordStepSize = 1.f / (220000.f / static_cast<float>(decimationFactor));
                convertVibrationValue = {
                        [](int16_t valueRaw) {
                            return static_cast<float>(valueRaw) * 0.001907349;
                        }
                };
                break;
            case RecordingMode::MFFT:
            case RecordingMode::AFFT:
                const uint8_t numberOfFFTAvg = readRecInfoFFTAveragesCount();
                samplesCount = 2048;
                recordStepSize = 110000.f / static_cast<float>(decimationFactor) / static_cast<float>(samplesCount);
                convertVibrationValue = {
                        [numberOfFFTAvg](int16_t valueRaw) {
                            return std::pow(2, static_cast<float>(valueRaw) / 2048) / numberOfFFTAvg * 0.9535;
                        }
                };
//                convertVibrationValue = {
//                        [](int16_t valueRaw) {
//                            return static_cast<float>(valueRaw);
//                        }
//                };
                break;
        }

        write(spi_commands::BUF_PNTR, 0);

        VibrationData vibrationData;
        vibrationData.recordingMode = currentRecordingMode;
        vibrationData.stepAxis = generateSteps(recordStepSize, samplesCount);
        vibrationData.xAxis = readBuffer(spi_commands::X_BUF, samplesCount, convertVibrationValue);
        vibrationData.yAxis = readBuffer(spi_commands::Y_BUF, samplesCount, convertVibrationValue);
        vibrationData.zAxis = readBuffer(spi_commands::Z_BUF, samplesCount, convertVibrationValue);

        return vibrationData;
    }

    std::vector<float> VibrationSensorModule::generateSteps(float stepSize, int samplesCount) {
        std::vector<float> stepAxis;
        stepAxis.reserve(samplesCount);

        for (int i = 0; i < samplesCount; ++i) {
            stepAxis.push_back(stepSize * i);
        }

        return stepAxis;
    }

    std::vector<float> VibrationSensorModule::readBuffer(SpiCommand cmd, int samplesCount,
                                                         const std::function<float(int16_t)> &convertVibrationValue) const {
        std::vector<float> axisData;
        axisData.reserve(samplesCount);

        // select page
        transferWaiting({0x80, cmd.pageId});
        transferWaiting({cmd.address, 0});

        for (int i = 0; i < (samplesCount - 1); ++i) {
            uint16_t resp = convert(transferWaiting({cmd.address, 0}));
            auto valueRaw = static_cast<int16_t>(resp);
            axisData.push_back(convertVibrationValue(valueRaw));
        }
        int16_t valueRaw = static_cast<int16_t>(convert(transferWaiting({0, 0})));
        axisData.push_back(convertVibrationValue(valueRaw));
        return axisData;
    }

    int VibrationSensorModule::readRecInfoFFTAveragesCount() const {
        return read(spi_commands::REC_INFO1) & 0xFF;
    }

    int VibrationSensorModule::readRecInfoDecimationFactor() const {
        int avgCnt = read(spi_commands::REC_INFO2) & 0x7;
        return static_cast<int>(pow(2, avgCnt));
    }

    bool
    VibrationSensorModule::activateMode(const RecordingConfig &recordingConfig, const RecordingMode &recordingMode,
                                        const WindowSetting &windowSetting) {
        // only modify SR0 as we only work with that. keep rest default.
        uint16_t avgCnt = 0x7420;
        avgCnt |= static_cast<uint8_t>(recordingConfig.decimationFactor);
        write(spi_commands::AVG_CNT, avgCnt);

        if (recordingConfig.firFilter == FIRFilter::CUSTOM) {
            writeCustomFIRFilterTaps(recordingConfig.customFilterTaps);

            // select filter bank F
            writeFIRFilter(FIRFilter::HIGH_PASS_10kHz);
        } else {
            writeFIRFilter(recordingConfig.firFilter);
        }

        return writeRecordingControl(recordingMode, windowSetting);
    }

    bool VibrationSensorModule::activateMode(const MFFTConfig &mfftConfig) {
        // only modify SR0 as we only work with that. keep rest default.
        uint16_t fftAvg1 = 0x0100;
        fftAvg1 |= mfftConfig.spectralAvgCount;
        write(spi_commands::FFT_AVG1, fftAvg1);

        return activateMode(mfftConfig, RecordingMode::MFFT, mfftConfig.windowSetting);
    }

    bool VibrationSensorModule::activateMode(const MTCConfig &mtcConfig) {
        return activateMode(mtcConfig, RecordingMode::MTC);
    }

    void VibrationSensorModule::writeFIRFilter(FIRFilter firFilter) {
        uint16_t filtCtrl = 0x0000;
        // set for every axis same filter
        filtCtrl |= static_cast<uint8_t>(firFilter);
        filtCtrl |= (static_cast<uint8_t>(firFilter) << 3);
        filtCtrl |= (static_cast<uint8_t>(firFilter) << 6);
        write(spi_commands::FILT_CTRL, filtCtrl);
    }

    void VibrationSensorModule::writeCustomFIRFilterTaps(std::array<int16_t, 32> customFilterTaps) {
        // store custom values in filter bank F
        for (int i = 0; i < customFilterTaps.size(); ++i) {
            write(spi_commands::FIR_COEFFS_F[i], static_cast<uint16_t>(customFilterTaps[i]));
        }
    }

    void VibrationSensorModule::triggerRecording() const {
        write(spi_commands::GLOB_CMD, 0x0800);
    }

    void VibrationSensorModule::activateExternalTrigger() const {
        write(spi_commands::MISC_CTRL, 0x1000);
    }



}
