#ifndef DUALBUTTON_H
#define DUALBUTTON_H

#include <QWidget>
#include "sensorinterface.h"
#include "statusled.h"

namespace Ui {
class DualButton;
}

/**
 * Implements the DualButton Widget.
 */
class DualButton : public SensorInterface
{
    Q_OBJECT

    StatusLed *led_l;
    StatusLed *led_r;
    uint64_t  buttonState;
    Ui::DualButton *ui;

public:
    explicit DualButton(QWidget *parent, const char *type, const char *uid);
    ~DualButton();

    /**
     * This method is called per state change from DualButton -> update UI.
     */
    virtual void notify(const stubserver::VisibleDeviceState &hint) override;

public slots:
    void updateUi(int);

    // 0 for LEFT, 1 for RIGHT button
    void buttonDown(int);
    void buttonUp(int);
};

#endif // DUALBUTTON_H
