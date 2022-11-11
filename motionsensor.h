#ifndef MOTIONSENSOR_H
#define MOTIONSENSOR_H

#include <QWidget>
#include "sensorinterface.h"

namespace Ui {
class MotionSensor;
}

/**
 * @brief The MotionSensor class
 *
 * Use for motion bricklets, hall effect (also some kind of motion) and tilt sensor
 */
class MotionSensor : public SensorInterface
{
    Q_OBJECT

    Ui::MotionSensor *ui;
    bool useCounter;
    bool showLeds;

public:
    explicit MotionSensor(QWidget *parent, const char *title);
    ~MotionSensor();

    // enables a label with a use counter (hall effect)
    void showUseCounter() {
        useCounter = true;
    }

    // enables additional three leds for motion sensor V2
    void showAllLeds() {
        showLeds = true;
    }

public slots:
    void updateUi(int);
    void toggleState();

    // new led value, signal emitted by SensorInterface
    void updateLed(bool);
};

#endif // MOTIONSENSOR_H
