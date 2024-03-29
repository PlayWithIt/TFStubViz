#ifndef SENSOR_H
#define SENSOR_H

#include <QAbstractSlider>
#include <QLabel>

#include "sensorinterface.h"

namespace Ui {
class Sensor;
class Potentiometer;
}

/**
 * Simple sensor UI: show a slider which moves up and down and shows
 * the current values additionally via label.
 */
class Sensor : public SensorInterface
{
    Q_OBJECT

    Ui::Sensor        *uiS;
    Ui::Potentiometer *uiP;
    QAbstractSlider   *sensor;
    QLabel            *labelMin;
    QLabel            *labelMax;
    QLabel            *label;

public:
    explicit Sensor(QWidget *parent, const char *title, bool rotary);
    ~Sensor();

    /**
     * Sets the additional label for the value type.
     * @param label - label string
     */
    void setValueLabel(const std::string &label) const;

public slots:
    // new sensor value from extern
    void updateUi(int);
    // new sensor value manually (other zero point handling)
    void sliderMoved(int);

    // new led value
    void updateLed(bool);

    // slider changed
    void actionTriggered(int action);
};

#endif // SENSOR_H
