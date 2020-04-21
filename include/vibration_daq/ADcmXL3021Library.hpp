//
// Created by Jonas Lauener on 04.03.20.
//

#pragma once

#include <cstdint>

namespace vibration_daq {
    struct SpiCommand {
        uint8_t pageId;
        uint8_t address;
        bool readFlag;
        bool writeFlag;
    };

    // generated with https://docs.google.com/spreadsheets/d/1Mb0L5uJfrP49SAjBWerlkgQbLD7uuCMY-8qP3WlNRI0/edit?usp=sharing
    namespace spi_commands {
        const SpiCommand PAGE_ID = {0x00, 0x00, true, false};
        const SpiCommand TEMP_OUT = {0x00, 0x02, true, false};
        const SpiCommand SUPPLY_OUT = {0x00, 0x04, true, false};
        const SpiCommand FFT_AVG1 = {0x00, 0x06, true, true};
        const SpiCommand FFT_AVG2 = {0x00, 0x08, true, true};
        const SpiCommand BUF_PNTR = {0x00, 0x0A, true, true};
        const SpiCommand REC_PNTR = {0x00, 0x0C, true, true};
        const SpiCommand X_BUF = {0x00, 0x0E, true, true};
        const SpiCommand Y_BUF = {0x00, 0x10, true, true};
        const SpiCommand Z_BUF = {0x00, 0x12, true, true};
        const SpiCommand X_ANULL = {0x00, 0x14, true, true};
        const SpiCommand Y_ANULL = {0x00, 0x16, true, true};
        const SpiCommand Z_ANULL = {0x00, 0x18, true, true};
        const SpiCommand REC_CTRL = {0x00, 0x1A, true, true};
        const SpiCommand REC_PRD = {0x00, 0x1E, true, true};
        const SpiCommand ALM_F_LOW = {0x00, 0x20, true, true};
        const SpiCommand ALM_F_HIGH = {0x00, 0x22, true, true};
        const SpiCommand ALM_X_MAG1 = {0x00, 0x24, true, true};
        const SpiCommand ALM_Y_MAG1 = {0x00, 0x26, true, true};
        const SpiCommand ALM_Z_MAG1 = {0x00, 0x28, true, true};
        const SpiCommand ALM_X_MAG2 = {0x00, 0x2A, true, true};
        const SpiCommand ALM_Y_MAG2 = {0x00, 0x2C, true, true};
        const SpiCommand ALM_Z_MAG2 = {0x00, 0x2E, true, true};
        const SpiCommand ALM_PNTR = {0x00, 0x30, true, true};
        const SpiCommand ALM_S_MAG = {0x00, 0x32, true, true};
        const SpiCommand ALM_CTRL = {0x00, 0x34, true, true};
        const SpiCommand DIO_CTRL = {0x00, 0x36, true, true};
        const SpiCommand FILT_CTRL = {0x00, 0x38, true, true};
        const SpiCommand AVG_CNT = {0x00, 0x3A, true, true};
        const SpiCommand DIAG_STAT = {0x00, 0x3C, true, false};
        const SpiCommand GLOB_CMD = {0x00, 0x3E, false, true};
        const SpiCommand ALM_X_STAT = {0x00, 0x40, true, false};
        const SpiCommand ALM_Y_STAT = {0x00, 0x42, true, false};
        const SpiCommand ALM_Z_STAT = {0x00, 0x44, true, false};
        const SpiCommand ALM_X_PEAK = {0x00, 0x46, true, false};
        const SpiCommand ALM_Y_PEAK = {0x00, 0x48, true, false};
        const SpiCommand ALM_Z_PEAK = {0x00, 0x4A, true, false};
        const SpiCommand TIME_STAMP_L = {0x00, 0x4C, true, false};
        const SpiCommand TIME_STAMP_H = {0x00, 0x4E, true, false};
        const SpiCommand REV_DAY = {0x00, 0x52, true, false};
        const SpiCommand YEAR_MON = {0x00, 0x54, true, false};
        const SpiCommand PROD_ID = {0x00, 0x56, true, true};
        const SpiCommand SERIAL_ID = {0x00, 0x58, true, true};
        const SpiCommand USER_SCRATCH = {0x00, 0x5A, true, true};
        const SpiCommand REC_FLSH_CNT = {0x00, 0x5C, true, false};
        const SpiCommand MISC_CTRL = {0x00, 0x64, true, true};
        const SpiCommand REC_INFO1 = {0x00, 0x66, true, false};
        const SpiCommand REC_INFO2 = {0x00, 0x68, true, false};
        const SpiCommand REC_CNT = {0x00, 0x6A, true, false};
        const SpiCommand ALM_X_FREQ = {0x00, 0x6C, true, false};
        const SpiCommand ALM_Y_FREQ = {0x00, 0x6E, true, false};
        const SpiCommand ALM_Z_FREQ = {0x00, 0x70, true, false};
        const SpiCommand TD_STAT_PNTR = {0x00, 0x72, true, true};
        const SpiCommand X_STATISTIC = {0x00, 0x74, true, false};
        const SpiCommand Y_STATISTIC = {0x00, 0x76, true, false};
        const SpiCommand Z_STATISTIC = {0x00, 0x78, true, false};
        const SpiCommand F_FREQ = {0x00, 0x7A, true, true};
        const SpiCommand ENDUR_LWR = {0x00, 0x7C, true, false};
        const SpiCommand ENDUR_UPR = {0x00, 0x7E, true, false};

        const std::vector<SpiCommand> FIR_COEFFS_A = {
                {0x01, 0x02, true, true},
                {0x01, 0x04, true, true},
                {0x01, 0x06, true, true},
                {0x01, 0x08, true, true},
                {0x01, 0x0A, true, true},
                {0x01, 0x0C, true, true},
                {0x01, 0x0E, true, true},
                {0x01, 0x10, true, true},
                {0x01, 0x12, true, true},
                {0x01, 0x14, true, true},
                {0x01, 0x16, true, true},
                {0x01, 0x18, true, true},
                {0x01, 0x1A, true, true},
                {0x01, 0x1C, true, true},
                {0x01, 0x1E, true, true},
                {0x01, 0x20, true, true},
                {0x01, 0x22, true, true},
                {0x01, 0x24, true, true},
                {0x01, 0x26, true, true},
                {0x01, 0x28, true, true},
                {0x01, 0x2A, true, true},
                {0x01, 0x2C, true, true},
                {0x01, 0x2E, true, true},
                {0x01, 0x30, true, true},
                {0x01, 0x32, true, true},
                {0x01, 0x34, true, true},
                {0x01, 0x36, true, true},
                {0x01, 0x38, true, true},
                {0x01, 0x3A, true, true},
                {0x01, 0x3C, true, true},
                {0x01, 0x3E, true, true},
                {0x01, 0x40, true, true}};

        const std::vector<SpiCommand> FIR_COEFFS_B = {
                {0x02, 0x02, true, true},
                {0x02, 0x04, true, true},
                {0x02, 0x06, true, true},
                {0x02, 0x08, true, true},
                {0x02, 0x0A, true, true},
                {0x02, 0x0C, true, true},
                {0x02, 0x0E, true, true},
                {0x02, 0x10, true, true},
                {0x02, 0x12, true, true},
                {0x02, 0x14, true, true},
                {0x02, 0x16, true, true},
                {0x02, 0x18, true, true},
                {0x02, 0x1A, true, true},
                {0x02, 0x1C, true, true},
                {0x02, 0x1E, true, true},
                {0x02, 0x20, true, true},
                {0x02, 0x22, true, true},
                {0x02, 0x24, true, true},
                {0x02, 0x26, true, true},
                {0x02, 0x28, true, true},
                {0x02, 0x2A, true, true},
                {0x02, 0x2C, true, true},
                {0x02, 0x2E, true, true},
                {0x02, 0x30, true, true},
                {0x02, 0x32, true, true},
                {0x02, 0x34, true, true},
                {0x02, 0x36, true, true},
                {0x02, 0x38, true, true},
                {0x02, 0x3A, true, true},
                {0x02, 0x3C, true, true},
                {0x02, 0x3E, true, true},
                {0x02, 0x40, true, true}
        };
        const std::vector<SpiCommand> FIR_COEFFS_C = {
                {0x03, 0x02, true, true},
                {0x03, 0x04, true, true},
                {0x03, 0x06, true, true},
                {0x03, 0x08, true, true},
                {0x03, 0x0A, true, true},
                {0x03, 0x0C, true, true},
                {0x03, 0x0E, true, true},
                {0x03, 0x10, true, true},
                {0x03, 0x12, true, true},
                {0x03, 0x14, true, true},
                {0x03, 0x16, true, true},
                {0x03, 0x18, true, true},
                {0x03, 0x1A, true, true},
                {0x03, 0x1C, true, true},
                {0x03, 0x1E, true, true},
                {0x03, 0x20, true, true},
                {0x03, 0x22, true, true},
                {0x03, 0x24, true, true},
                {0x03, 0x26, true, true},
                {0x03, 0x28, true, true},
                {0x03, 0x2A, true, true},
                {0x03, 0x2C, true, true},
                {0x03, 0x2E, true, true},
                {0x03, 0x30, true, true},
                {0x03, 0x32, true, true},
                {0x03, 0x34, true, true},
                {0x03, 0x36, true, true},
                {0x03, 0x38, true, true},
                {0x03, 0x3A, true, true},
                {0x03, 0x3C, true, true},
                {0x03, 0x3E, true, true},
                {0x03, 0x40, true, true}
        };
        const std::vector<SpiCommand> FIR_COEFFS_D = {
                {0x04, 0x02, true, true},
                {0x04, 0x04, true, true},
                {0x04, 0x06, true, true},
                {0x04, 0x08, true, true},
                {0x04, 0x0A, true, true},
                {0x04, 0x0C, true, true},
                {0x04, 0x0E, true, true},
                {0x04, 0x10, true, true},
                {0x04, 0x12, true, true},
                {0x04, 0x14, true, true},
                {0x04, 0x16, true, true},
                {0x04, 0x18, true, true},
                {0x04, 0x1A, true, true},
                {0x04, 0x1C, true, true},
                {0x04, 0x1E, true, true},
                {0x04, 0x20, true, true},
                {0x04, 0x22, true, true},
                {0x04, 0x24, true, true},
                {0x04, 0x26, true, true},
                {0x04, 0x28, true, true},
                {0x04, 0x2A, true, true},
                {0x04, 0x2C, true, true},
                {0x04, 0x2E, true, true},
                {0x04, 0x30, true, true},
                {0x04, 0x32, true, true},
                {0x04, 0x34, true, true},
                {0x04, 0x36, true, true},
                {0x04, 0x38, true, true},
                {0x04, 0x3A, true, true},
                {0x04, 0x3C, true, true},
                {0x04, 0x3E, true, true},
                {0x04, 0x40, true, true}
        };
        const std::vector<SpiCommand> FIR_COEFFS_E = {
                {0x05, 0x02, true, true},
                {0x05, 0x04, true, true},
                {0x05, 0x06, true, true},
                {0x05, 0x08, true, true},
                {0x05, 0x0A, true, true},
                {0x05, 0x0C, true, true},
                {0x05, 0x0E, true, true},
                {0x05, 0x10, true, true},
                {0x05, 0x12, true, true},
                {0x05, 0x14, true, true},
                {0x05, 0x16, true, true},
                {0x05, 0x18, true, true},
                {0x05, 0x1A, true, true},
                {0x05, 0x1C, true, true},
                {0x05, 0x1E, true, true},
                {0x05, 0x20, true, true},
                {0x05, 0x22, true, true},
                {0x05, 0x24, true, true},
                {0x05, 0x26, true, true},
                {0x05, 0x28, true, true},
                {0x05, 0x2A, true, true},
                {0x05, 0x2C, true, true},
                {0x05, 0x2E, true, true},
                {0x05, 0x30, true, true},
                {0x05, 0x32, true, true},
                {0x05, 0x34, true, true},
                {0x05, 0x36, true, true},
                {0x05, 0x38, true, true},
                {0x05, 0x3A, true, true},
                {0x05, 0x3C, true, true},
                {0x05, 0x3E, true, true},
                {0x05, 0x40, true, true}
        };
        const std::vector<SpiCommand> FIR_COEFFS_F = {
                {0x06, 0x02, true, true},
                {0x06, 0x04, true, true},
                {0x06, 0x06, true, true},
                {0x06, 0x08, true, true},
                {0x06, 0x0A, true, true},
                {0x06, 0x0C, true, true},
                {0x06, 0x0E, true, true},
                {0x06, 0x10, true, true},
                {0x06, 0x12, true, true},
                {0x06, 0x14, true, true},
                {0x06, 0x16, true, true},
                {0x06, 0x18, true, true},
                {0x06, 0x1A, true, true},
                {0x06, 0x1C, true, true},
                {0x06, 0x1E, true, true},
                {0x06, 0x20, true, true},
                {0x06, 0x22, true, true},
                {0x06, 0x24, true, true},
                {0x06, 0x26, true, true},
                {0x06, 0x28, true, true},
                {0x06, 0x2A, true, true},
                {0x06, 0x2C, true, true},
                {0x06, 0x2E, true, true},
                {0x06, 0x30, true, true},
                {0x06, 0x32, true, true},
                {0x06, 0x34, true, true},
                {0x06, 0x36, true, true},
                {0x06, 0x38, true, true},
                {0x06, 0x3A, true, true},
                {0x06, 0x3C, true, true},
                {0x06, 0x3E, true, true},
                {0x06, 0x40, true, true}
        };
    }
}