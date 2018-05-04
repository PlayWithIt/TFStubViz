#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

#include <QCheckBox>

#include "statusled.h"
#include "visualizationwidget.h"

/**
 * Base class for object that can be used as sensors (even button input
 * is some kind of sensor). This class handles a manual control: a
 * derived class can have a checkbox to enable manual control.
 */
class SensorInterface : public QWidget, public StatusLed, public VisualizationWidget
{
    Q_OBJECT

protected:
    QCheckBox *checkBox;

    static const int MAX_INTERNAL_SENSORS = 4;

    int      currentValue;
    unsigned counter;
    int      min, max;
    bool     minMaxSet;
    bool     manualControl;

    void setCheckBox(QCheckBox *check) {
        checkBox = check;
    }

public:
    explicit SensorInterface(QWidget *p, QWidget *_statusLED = NULL);

    //----- VisualizationClient
    virtual void notify(const stubserver::VisibleDeviceState &state) override;
    virtual bool useAsInputSource(unsigned sensorNo) const override;
    virtual int64_t getInputState(unsigned sensorNo) const override;

signals:
    void valueChanged(int);
    void ledChanged(bool);

public slots:
    void checkBoxClicked(int);
    void activateManualControl();

};

#endif // SENSORINTERFACE_H
