#include "statusled.h"

StatusLed::StatusLed(QWidget *_statusLED)
    : statusLED(_statusLED), ledOn(false)
{
}


/**
 * Sets the background color for the given LED to signal that the LED is on or off
 * @param on on = true
 * @param updateUi if false, the widget is NOT updated (necessary in event cycles)
 */
void StatusLed::setLedOn(bool on, bool updateUi)
{
    // printf("StatusLed::setLedOn(%d, %d), led = %p\n", on, updateUi, statusLED);

    ledOn = on;
    if (statusLED == NULL || !updateUi)
        return;

    if (on)
        statusLED->setStyleSheet("background-color:blue;");
    else
        statusLED->setStyleSheet("background-color:gray;");
}
