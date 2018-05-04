#ifndef STATUSLED_H
#define STATUSLED_H

#include <QWidget>

/**
 * Helper class to support a status LED on bricks and bricklets.
 *
 * @brief The StatusLed class
 */
class StatusLed
{
protected:
    QWidget *statusLED;
    bool    ledOn;

public:
    StatusLed(QWidget *_statusLED);

    /**
     * Sets the background color for the given LED to signal that the LED is on or off
     * @param on on = true
     * @param updateUi if false, the widget is NOT updated (necessary in event cycles)
     */
    void setLedOn(bool on, bool updateUi = true);

    /**
     * @return true if the status LED is on
     * @brief isLedOn
     */
    bool isLedOn() const {
        return ledOn;
    }
};

#endif // STATUSLED_H
