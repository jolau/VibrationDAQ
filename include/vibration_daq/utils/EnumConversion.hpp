//
// Created by Jonas Lauener on 18.03.20.
//

#pragma once

namespace vibration_daq {
    namespace Enum {
        template<typename T>
        inline static const bool
        convert(const T &fromEnum, std::string &toEnumString, const std::map<T, std::string> &enumStringMap) {
            auto it = enumStringMap.find(fromEnum);
            if (it != enumStringMap.end()) {
                toEnumString = it->second;
                return true;
            }
            return false;
        }

        template<typename T>
        inline static const std::string toString(const T &fromEnum, const std::map<T, std::string> &enumStringMap) {
            std::string enumString;
            if (convert(fromEnum, enumString, enumStringMap)) {
                return enumString;
            }
            return "";
        }

        template<typename T>
        inline static const bool
        convert(const std::string &fromEnumString, T &toEnum, const std::map<T, std::string> &enumStringMap
        ) {
            for (const auto &it : enumStringMap) {
                if (it.second == fromEnumString) {
                    toEnum = it.first;
                    return true;
                }
            }
            return false;
        }
    }
}