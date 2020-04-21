//
// Created by Jonas Lauener on 13.03.20.
//

#pragma once

#include <map>
#include "../utils/EnumConversion.hpp"

namespace vibration_daq {
    enum class RecordingMode {
        MFFT = 0u,
        AFFT = 1u,
        MTC = 2u,
        RTS = 3u
    };

    namespace Enum {
        const std::map<RecordingMode, std::string> RECORDING_MODE_STRING_MAP{
                {RecordingMode::MFFT, "MFFT"},
                {RecordingMode::AFFT, "AFFT"},
                {RecordingMode::MTC,  "MTC"},
                {RecordingMode::RTS,  "RTS"}
        };

        inline const std::string toString(const RecordingMode &fromEnum) {
            return toString(fromEnum, RECORDING_MODE_STRING_MAP);
        }

        inline static const bool convert(const RecordingMode &fromEnum, std::string &toEnumString) {
            return convert(fromEnum, toEnumString, RECORDING_MODE_STRING_MAP);
        }

        inline static const bool convert(const std::string &fromEnumString, RecordingMode &toEnum) {
            return convert(fromEnumString, toEnum, RECORDING_MODE_STRING_MAP);
        }
    };
}