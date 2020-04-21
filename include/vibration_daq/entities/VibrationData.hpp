//
// Created by Jonas Lauener on 05.03.20.
//

#pragma once

#include "RecordingMode.hpp"

namespace vibration_daq {
    struct VibrationData {
        RecordingMode recordingMode;
        std::vector<float> stepAxis; // time resp. frequency axis
        std::vector<float> xAxis;
        std::vector<float> yAxis;
        std::vector<float> zAxis;
    };
}