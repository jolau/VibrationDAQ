//
// Created by Jonas Lauener on 05.03.20.
//

#pragma once

#include <iostream>

namespace vibration_daq {
    /**
     * 2-byte buffer
     */
    typedef std::array<uint8_t, 2> WordBuffer;

    inline static uint16_t convert(const WordBuffer &wordBuffer)  {
        return (wordBuffer[0] << 8) | wordBuffer[1];
    }

    inline static std::string getHexString(uint8_t num) {
        char str[6];
        sprintf(str, "0x%02X ", num);
        return str;
    }

    inline static std::string getHexString(uint16_t num) {
        char str[6];
        sprintf(str, "0x%04X ", num);
        return str;
    }

    inline static std::string getHexString(std::array<uint8_t, 2> num) {
        char str[6];
        sprintf(str, "0x%02X%02X ", num[0], num[1]);
        return str;
    }
}