//
// Created by Jonas Lauener on 18.03.20.
//

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
