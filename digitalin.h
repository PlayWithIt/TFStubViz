#ifndef DIGITALIN_H
#define DIGITALIN_H

#include <QWidget>
#include "sensorinterface.h"
#include "statusled.h"

namespace Ui {
class DigitalIn;
}

/**
 * Implements the DigitalIn Widget.
 */
class DigitalIn : public SensorInterface
{
    Q_OBJECT

    StatusLed *led0;
    StatusLed *led1;
    StatusLed *led2;
    StatusLed *led3;
    uint64_t  buttonState;
    Ui::DigitalIn *ui;

public:
    explicit DigitalIn(QWidget *parent, const char *type, const char *uid);
    ~DigitalIn();

    /**
     * This method is called per state change from DigitalIn -> update UI.
     */
    virtual void notify(const stubserver::VisibleDeviceState &hint) override;

public slots:
    void updateUi(int);

    // parameter is the button index
    void buttonClick(int);
};

#endif // DIGITALIN_H
