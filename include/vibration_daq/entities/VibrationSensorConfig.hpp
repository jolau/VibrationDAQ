/* Copyright (c) 2020, Jonas Lauener & Wingtra AG
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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