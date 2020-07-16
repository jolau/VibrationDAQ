/* Copyright (c) 2020, Jonas Lauener & Wingtra AG
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once
namespace vibration_daq {
    enum class WindowSetting {
        RECTANGULAR = 0b00,
        HANNING = 0b01,
        FLAT_TOP = 0b10
    };

    namespace Enum {
        const std::map<WindowSetting, std::string> WINDOW_SETTING_STRING_MAP{
                {WindowSetting::RECTANGULAR, "RECTANGULAR"},
                {WindowSetting::HANNING , "HANNING"},
                {WindowSetting::FLAT_TOP , "FLAT_TOP"}
        };

        inline const std::string toString(const WindowSetting &windowSetting) {
            return toString(windowSetting, WINDOW_SETTING_STRING_MAP);
        }

        inline static const bool convert(const WindowSetting &fromWindowSetting, std::string &toWindowSettingString) {
            return convert(fromWindowSetting, toWindowSettingString, WINDOW_SETTING_STRING_MAP);
        }

        inline static const bool convert(const std::string &fromWindowSettingString, WindowSetting &toWindowSetting) {
            return convert(fromWindowSettingString, toWindowSetting, WINDOW_SETTING_STRING_MAP);
        }
    };
}