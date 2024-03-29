#ifndef DUALSENSOR_H
#define DUALSENSOR_H

#include "sensor.h"
#include "visualizationwidget.h"

class DualSensor : public QWidget, public VisualizationWidget
{
    Sensor *sensor0;
    Sensor *sensor1;

public:
    DualSensor(QWidget *parent, const char *type, const char *uid);
    ~DualSensor();

    void setValueLabels(const std::string &left, const std::string &right);

    /**
     * @brief setLedOn turn the first status LED on or OFF.
     * @param on if true, LED is on
     */
    void setLed1On(bool on) {
        sensor0->setLedOn(on);
    }

    /**
     * @brief setLed2On turn the second status LED on or OFF.
     * @param on if true, LED is on
     */
    void setLed2On(bool on) {
        sensor1->setLedOn(on);
    }

    //----- VisualizationWidget
    virtual void setStackParameter(char position, const std::string &parent) override;

    //----- VisualizationClient
    virtual void notify(const stubserver::VisibleDeviceState &state) override;
    virtual bool useAsInputSource(unsigned sensorNo) const override;
    virtual int64_t getInputState(unsigned sensorNo) const override;
};

#endif // DUALSENSOR_H
