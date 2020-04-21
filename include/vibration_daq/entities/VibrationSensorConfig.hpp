//
// Created by Jonas Lauener on 20.03.20.
//

#pragma once

namespace vibration_daq {
    struct VibrationSensorConfig {
        std::string name;
        int busyPin;
        int resetPin;
        std::string spiPath;
        RecordingMode recordingMode;
        MFFTConfig mfftConfig;
        MTCConfig mtcConfig;
    };
}