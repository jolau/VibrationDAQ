/* Copyright (c) 2020, Jonas Lauener & Wingtra AG
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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