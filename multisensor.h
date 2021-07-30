#ifndef MULTISENSOR_H
#define MULTISENSOR_H

#include <vector>

#include "sensor.h"
#include "visualizationwidget.h"

/**
 * A sensor widget with multiple sensors (e.g. 2 or 6 independant values),
 * this minimum sensor count is 2 (1 is not allowed!).
 *
 * @brief The MultiSensor class
 */
class MultiSensor : public QWidget, public VisualizationWidget
{
    std::vector<Sensor*> sensors;

public:
    MultiSensor(QWidget *parent, const char *type, unsigned sensorCount);
    ~MultiSensor();

    void setValueLabel(unsigned sensor, const std::string &label);

    /**
     * @brief setLedOn turn the status LED on or OFF (for Bricks).
     * @param on if true, LED is on
     */
    void setLedOn(bool on) {
        sensors[0]->setLedOn(on);
    }

    //----- VisualizationWidget
    virtual void setStackParameter(char position, const std::string &parent) override;

    //----- VisualizationClient
    virtual void notify(const stubserver::VisibleDeviceState &state) override;
    virtual bool useAsInputSource(unsigned sensorNo) const override;
    virtual int64_t getInputState(unsigned sensorNo) const override;
};

#endif // MULTISENSOR_H
