#ifndef MOTIONSENSOR_H
#define MOTIONSENSOR_H

#include <QWidget>
#include "sensorinterface.h"

namespace Ui {
class MotionSensor;
}

class MotionSensor : public SensorInterface
{
    Q_OBJECT

    Ui::MotionSensor *ui;
    bool useCounter;

public:
    explicit MotionSensor(QWidget *parent, const char *type, const char *uid);
    ~MotionSensor();

    void showUseCounter() {
        useCounter = true;
    }

public slots:
    void updateUi(int);
    void toggleState();
};

#endif // MOTIONSENSOR_H
