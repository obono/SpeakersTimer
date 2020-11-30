#pragma once

#include <Arduino.h>

#define DATA_SIZE   5
#define DIGITS      4

enum PLAY_MODE_T : uint8_t {
    PLAY_MODE_NONE = 0,
    PLAY_MODE_PLAY,
    PLAY_MODE_PAUSE,
};

void initialize(void);
void setDigitByChar(int8_t pos, char c);
void setDigitByPattern(int8_t pos, uint8_t pattern);
void setDigitsByNumber(uint16_t number, int8_t digits);
void setDigitsByString(char *pStr);
void setPlayMode(PLAY_MODE_T playMode);
void setDelimiter(bool isOn);
void setAllByRawData(uint8_t *p);
