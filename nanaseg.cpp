#include "nanaseg.h"

/*
    Arduino: d1   d2   d3   d4   d5   d6   d7   d8   d9   d10  d11  d12
    Module : seg0 seg1 seg2 seg3 seg4 seg5 seg6 col0 col1 col2 col3 col4

    Column 0~4: seg0        Column 4: seg0: play mark
                ----                  seg1: pause mark
          seg5 |    | seg1            seg2: 'USB'
                ---- seg6             seg3: 'SD'
          seg4 |    | seg2            seg4: delimiter
                ----                  seg5: 'FM'
                seg3                  seg6: 'MP3'
*/

#define MISC_IDX    4

#define getPattern(idx)     pgm_read_byte(&charPattern[idx])
#define getPatternByChar(c) (((c) >= '0' || (c) <= '_') ? getPattern((c) - '0') : 0)

PROGMEM static const uint8_t charPattern[] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00100111,
    0b01111111, 0b01101111, 0b01000001, 0b01000101, 0b01011000, 0b01001000, 0b01001100, 0b01010011,
    0b01111011, 0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001, 0b01110001, 0b00111101,
    0b01110110, 0b00110000, 0b00001110, 0b01111000, 0b00111000, 0b00110111, 0b01010100, 0b01011100,
    0b01110011, 0b01100111, 0b01010000, 0b00101101, 0b00110001, 0b00111110, 0b00011100, 0b01101010,
    0b01000110, 0b01101110, 0b00011011, 0b00111001, 0b01100000, 0b00001111, 0b00000001, 0b00001000,
};

static volatile uint8_t column;
static volatile uint8_t data[DATA_SIZE];

void initialize(void)
{
    DDRD  |= 0b11111110; // Set pin1~7 as output
    DDRB  |= 0b00011111; // Set pin8~12 as output
    PORTB &= 0b11100000;

    column = 0;
    memset((void *)data, 0, DATA_SIZE);

    TCCR1A = 0b00000000; // COM1A=0, COM1B=0, WGM1=0100
    TCCR1B = 0b00001011; // ICNC1=0, ICES1=0, CS1=011
    OCR1A = (F_CPU >> 6) / 250 - 1; // 4ms / digit
    TCNT1 = 0;
    bitSet(TIMSK1, OCIE1A); // enable interrupt
}

void setDigitByChar(int8_t pos, char c)
{
    if (pos >= 0 && pos < DIGITS) data[pos] = getPatternByChar(c);
}

void setDigitByPattern(int8_t pos, uint8_t pattern)
{
    if (pos >= 0 && pos <= DATA_SIZE) data[pos] = pattern;
}

void setDigitsByNumber(uint16_t number, int8_t digits)
{
    for (int8_t pos = DIGITS - 1; pos >= 0; pos--) {
        data[pos] = (number > 0 || digits > 0) ? getPattern(number % 10) : 0;
        digits--;
        number /= 10;
    }
}

void setDigitsByString(char *pStr)
{
    for (int8_t pos = 0; pos < DIGITS; pos++) {
        char c = *pStr++;
        data[pos] = getPatternByChar(c);
    }
}

void setPlayMode(PLAY_MODE_T playMode)
{
    data[MISC_IDX] = data[MISC_IDX] & 0b11111100 | (playMode & 0b00000011);
}


void setDelimiter(bool isOn)
{
    if (isOn) {
        bitSet(data[MISC_IDX], 4);
    } else {
        bitClear(data[MISC_IDX], 4);
    }
}

void setAllByRawData(uint8_t *p)
{
    memcpy((void *)data, p, DATA_SIZE);
}

ISR(TIMER1_COMPA_vect)
{
    PORTB &= 0b11100000;
    PORTD = PORTD & 0b00000001 | (~data[column] << 1);
    bitSet(PORTB, column);
    column = (column + 1) % DATA_SIZE;
}
