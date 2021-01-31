#ifndef LEDBUTTON_H
#define LEDBUTTON_H

#include <QWidget>
#include "sensorinterface.h"
#include "statusled.h"

namespace Ui {
class LedButton;
}

/**
 * Implements the LedButton Widget.
 */
class LedButton : public SensorInterface
{
    Q_OBJECT

    StatusLed *led_l;
    Ui::LedButton *ui;
    unsigned red, green, blue;

public:
    explicit LedButton(QWidget *parent, const char *type, const char *uid);
    ~LedButton() override;

    /**
     * This method is called per state change from LedButton -> update UI.
     */
    virtual void notify(const stubserver::VisibleDeviceState &hint) override;

public slots:
    void updateUi(int);

    void buttonDown();
    void buttonUp();
};

#endif // LEDBUTTON_H
