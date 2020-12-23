#include <IRremote.h>
#include "nanaseg.h"

#define FPS 8

enum STATE_T : int8_t {
    STATE_ENTER = 0,
    STATE_READY,
    STATE_PLAY,
    STATE_PAUSE,
    STATE_TIMEOUT,
};

enum : int8_t {
    KEY_UNKNOWN = -1,
    KEY_0 = 0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_ASTERISK,
    KEY_POUND,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_OK,
    KEY_MAX
};

PROGMEM static const uint32_t codeTable[KEY_MAX] = {
    0x00FF9867, 0x00FFA25D, 0x00FF629D, 0x00FFE21D, 0x00FF22DD, // 0 ~ 4
    0x00FF02FD, 0x00FFC23D, 0x00FFE01F, 0x00FFA857, 0x00FF906F, // 5 ~ 9
    0x00FF6897, 0x00FFB04F,                                     // asterisk, pound
    0x00FF18E7, 0x00FF4AB5, 0x00FF10EF, 0x00FF5AA5,             // up, down, left, right
    0x00FF38C7,                                                 // ok
};

static void     handleKey(int8_t key);
static void     updateDiaplay(void);

static IRrecv   IrReceiver(A0);
static STATE_T  state = STATE_ENTER;
static uint16_t enteredNumber = 0, currentNumber;
static uint8_t  oddFrames = FPS;
static uint32_t targetTime;

/*---------------------------------------------------------------------------*/

using namespace Nanaseg;

void setup(void)
{
    initialize();
    IrReceiver.enableIRIn();  // Start the receiver
    IrReceiver.blink13(true); // Enable feedback LED
    targetTime = millis();
}

void loop(void)
{
    if (state != STATE_PAUSE && --oddFrames == 0) {
        if (state == STATE_PLAY) {
            currentNumber -= (currentNumber % 100 == 0) ? 41 : 1;
            if (currentNumber == 0) state = STATE_TIMEOUT;
        }
        oddFrames = FPS;
    }

    if (IrReceiver.decode()) {
        if (IrReceiver.results.decode_type == NEC && IrReceiver.results.bits == 32) {
            for (int8_t i = 0; i < KEY_MAX; i++) {
                if (IrReceiver.results.value == pgm_read_dword(&codeTable[i])) {
                    handleKey(i);
                    break;
                }
            }
        }
        IrReceiver.resume();
    }

    updateDiaplay();

    targetTime += 1000 / FPS;
    uint32_t delayTime = targetTime - millis();
    if (!bitRead(delayTime, 31)) delay(delayTime);
}

/*---------------------------------------------------------------------------*/

static void handleKey(int8_t key)
{
    switch (state) {
    case STATE_ENTER:
    case STATE_READY:
        if (key >= KEY_0 && key <= KEY_9) {
            if (state == STATE_READY) {
                enteredNumber = 0;
                state = STATE_ENTER;
            }
            enteredNumber = (enteredNumber % 1000) * 10 + key;
        }
        if (key == KEY_ASTERISK) {
            enteredNumber = 0;
            state = STATE_ENTER;
        }
        if (key == KEY_POUND) {
            if (enteredNumber > 0 && enteredNumber % 100 < 60) {
                currentNumber = enteredNumber;
                oddFrames = FPS;
                state = STATE_PLAY;
            }
        }
        break;
    case STATE_PLAY:
        if (key == KEY_POUND) state = STATE_PAUSE;
        break;
    case STATE_PAUSE:
        if (key == KEY_ASTERISK) state = STATE_READY;
        if (key == KEY_POUND) state = STATE_PLAY;
        break;
    case STATE_TIMEOUT:
        if (key == KEY_ASTERISK || key == KEY_POUND) state = STATE_READY;
        break;
    }
}

static void updateDiaplay(void)
{
    if (state == STATE_ENTER || state == STATE_READY) {
        if (state == STATE_ENTER && oddFrames > FPS * 3 / 4) {
            setDigitsByNumber(0, 0);
        } else {
            setDigitsByNumber(enteredNumber, (state == STATE_ENTER) ? 4 : 2);
        }
        setDelimiter(true);
        setPlayMode(PLAY_MODE_NONE);
    } else {
        if (state == STATE_TIMEOUT && (oddFrames & FPS / 4)) {
            setDigitsByNumber(0, 0);
        } else {
            setDigitsByNumber(currentNumber, 2);
        }
        setDelimiter(state != STATE_PLAY || oddFrames > FPS / 2);
        setPlayMode((state == STATE_PAUSE) ? PLAY_MODE_PAUSE : PLAY_MODE_PLAY);
    }
}
