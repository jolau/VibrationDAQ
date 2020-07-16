/* Copyright (c) 2020, Jonas Lauener & Wingtra AG
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <array>
#include "DecimationFactor.hpp"
#include "FIRFilter.hpp"
#include "WindowSetting.hpp"

namespace vibration_daq {
    struct RecordingConfig {
        DecimationFactor decimationFactor = DecimationFactor::FACTOR_1;
        FIRFilter firFilter = FIRFilter::NO_FILTER;
        std::array<int16_t, 32> customFilterTaps = {};
    };

    struct MTCConfig : RecordingConfig {

    };

    struct MFFTConfig : RecordingConfig {
        int spectralAvgCount = 1; // 1-255
        WindowSetting windowSetting = WindowSetting::HANNING;
    };

}
