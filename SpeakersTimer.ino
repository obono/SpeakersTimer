#include <IRremote.h>
#include "nanaseg.h"

/*
    1   0x00FFA25D
    2   0x00FF629D
    3   0x00FFE21D
    4   0x00FF22DD
    5   0x00FF02FD
    6   0x00FFC23D
    7   0x00FFE01F
    8   0x00FFA857
    9   0x00FF906F
    0   0x00FF9867
    *   0x00FF6897
    #   0x00FFB04F
    up  0x00FF18E7
    dn  0x00FF4AB5
    lf  0x00FF10EF
    rt  0x00FF5AA5
    OK  0x00FF38C7
*/

IRrecv IrReceiver(A0);
uint32_t targetTime;

/*---------------------------------------------------------------------------*/

void setup(void)
{
    delay(3000);
    initialize();
    IrReceiver.enableIRIn();  // Start the receiver
    IrReceiver.blink13(true); // Enable feedback LED
    targetTime = millis();
}

void loop(void)
{
    static uint16_t number = 0;
    static bool isDelimiterOn = false;

    if (IrReceiver.decode()) {
        if (IrReceiver.results.decode_type == NEC && IrReceiver.results.bits == 32) {
            switch (IrReceiver.results.value) {
                case 0x00FF6897: setPlayMode(PLAY_MODE_PLAY);  break;
                case 0x00FFB04F: setPlayMode(PLAY_MODE_PAUSE); break;
                case 0x00FF38C7: setPlayMode(PLAY_MODE_NONE);  break;
                default: break;
            }
        }
        IrReceiver.resume();
    }
    setDelimiter(isDelimiterOn);
    setDigitsByNumber(number, 4);
    isDelimiterOn = !isDelimiterOn;
    if (isDelimiterOn) number++;

    targetTime += 500;
    uint32_t delayTime = targetTime - millis();
    if (!bitRead(delayTime, 31)) delay(delayTime);
}
