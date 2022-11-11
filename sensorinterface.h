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

    int      currentValue;
    unsigned counter;
    int      min, max;
    int      zeroPoint;
    bool     minMaxSet;
    bool     manualControl;
    uint8_t  led1, led2, led3, led4;    // multiple purpose e.g. motion detector leds

    void setCheckBox(QCheckBox *check) {
        checkBox = check;
    }

public:
    explicit SensorInterface(QWidget *p, QWidget *_statusLED = nullptr);

    //----- VisualizationClient ---------------------------

    /**
     * notify() is called after the VisualizationClient (this object) is connected to an
     * instance of a 'SimulatedDevice'. The SimulatedDevice then sends the current value
     * and min / max initially.
     *
     * After the initial connect, this method is called when the value is not controlled
     * via UI but via ValueProvider and the UI just needs to display the current value.
     */
    virtual void notify(const stubserver::VisibleDeviceState &state) override;

    /**
     * This medthod is called periodically by the instance of a 'SimulatedDevice' in order
     * to check if the value comes from the UI or not. If this method returns true, this
     * UI object defines the sensor value.
     */
    virtual bool useAsInputSource(unsigned sensorNo) const override;

    /**
     * Return the current state (value) which is controlled via UI.
     */
    virtual int64_t getInputState(unsigned sensorNo) const override;

    /**
     * Set min / max directly, not via notify().
     */
    void setMinMax(int _min, int _max);

signals:
    void valueChanged(int);
    void ledChanged(bool);

public slots:
    void checkBoxClicked(int);
    void activateManualControl();

};

#endif // SENSORINTERFACE_H
