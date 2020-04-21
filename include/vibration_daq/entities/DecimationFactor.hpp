//
// Created by Jonas Lauener on 18.03.20.
//

#pragma once

#include <map>
#include "../utils/EnumConversion.hpp"

namespace vibration_daq {
    enum class DecimationFactor {
        FACTOR_1 = 0,
        FACTOR_2 = 1,
        FACTOR_4 = 2,
        FACTOR_8 = 3,
        FACTOR_16 = 4,
        FACTOR_32 = 5,
        FACTOR_64 = 6,
        FACTOR_128 = 7
    };

    namespace Enum {
        const std::map<DecimationFactor, std::string> DECIMATION_FACTOR_STRING_MAP{
                {DecimationFactor::FACTOR_1,   "FACTOR_1"},
                {DecimationFactor::FACTOR_2,   "FACTOR_2"},
                {DecimationFactor::FACTOR_4,   "FACTOR_4"},
                {DecimationFactor::FACTOR_8,   "FACTOR_8"},
                {DecimationFactor::FACTOR_16,  "FACTOR_16"},
                {DecimationFactor::FACTOR_32,  "FACTOR_32"},
                {DecimationFactor::FACTOR_64,  "FACTOR_64"},
                {DecimationFactor::FACTOR_128, "FACTOR_128"}
        };

        inline const std::string toString(const DecimationFactor &decimationFactor) {
            return toString(decimationFactor, DECIMATION_FACTOR_STRING_MAP);
        }

        inline const bool convert(const DecimationFactor &fromDecimationFactor, std::string &toDecimationFactorString) {
            return convert(fromDecimationFactor, toDecimationFactorString, DECIMATION_FACTOR_STRING_MAP);
        }

        inline const bool convert(const std::string &fromDecimationFactorString, DecimationFactor &toDecimationFactor) {
            return convert(fromDecimationFactorString, toDecimationFactor, DECIMATION_FACTOR_STRING_MAP);
        }
    };
}