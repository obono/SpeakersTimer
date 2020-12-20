#pragma once

#include <Arduino.h>

namespace Nanaseg
{
    enum PLAY_MODE_T : uint8_t {
        PLAY_MODE_NONE = 0,
        PLAY_MODE_PLAY,
        PLAY_MODE_PAUSE,
    };
    const uint8_t DATA_SIZE = 5;
    const uint8_t DIGITS = 4;
    void initialize(void);
    void setDigitByChar(int8_t pos, char c);
    void setDigitByPattern(int8_t pos, uint8_t pattern);
    void setDigitsByNumber(uint16_t number, int8_t digits);
    void setDigitsByString(char *pStr);
    void setPlayMode(PLAY_MODE_T playMode);
    void setDelimiter(bool isOn);
    void setAllByRawData(uint8_t *p);
};
