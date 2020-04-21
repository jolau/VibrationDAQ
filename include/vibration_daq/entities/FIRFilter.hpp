//
// Created by Jonas Lauener on 18.03.20.
//

#pragma once
namespace vibration_daq {
    enum class FIRFilter {
        NO_FILTER = 0b000,
        LOW_PASS_1kHz = 0b001, // bank A
        LOW_PASS_5kHz = 0b010, // bank B
        LOW_PASS_10kHz = 0b011, // bank C
        HIGH_PASS_1kHz = 0b100, // bank D
        HIGH_PASS_5kHz = 0b101, // bank E
        HIGH_PASS_10kHz = 0b110, // bank F
        CUSTOM
    };

    namespace Enum {
        const std::map<FIRFilter, std::string> FIR_FILTER_STRING_MAP{
                {FIRFilter::NO_FILTER, "NO_FILTER"},
                {FIRFilter::LOW_PASS_1kHz , "LOW_PASS_1kHz"},
                {FIRFilter::LOW_PASS_5kHz , "LOW_PASS_5kHz"},
                {FIRFilter::LOW_PASS_10kHz, "LOW_PASS_10kHz"},
                {FIRFilter::HIGH_PASS_1kHz, "HIGH_PASS_1kHz"},
                {FIRFilter::HIGH_PASS_5kHz, "HIGH_PASS_5kHz"},
                {FIRFilter::HIGH_PASS_10kHz, "HIGH_PASS_10kHz"},
                {FIRFilter::CUSTOM , "CUSTOM"}
        };

        inline const std::string toString(const FIRFilter &firFilter) {
            return toString(firFilter, FIR_FILTER_STRING_MAP);
        }

        inline static const bool convert(const FIRFilter &fromFIRFilter, std::string &toFIRFilterString) {
            return convert(fromFIRFilter, toFIRFilterString, FIR_FILTER_STRING_MAP);
        }

        inline static const bool convert(const std::string &fromFIRFilterString, FIRFilter &toFIRFilter) {
            return convert(fromFIRFilterString, toFIRFilter, FIR_FILTER_STRING_MAP);
        }
    };
}