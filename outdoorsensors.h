#ifndef OUTDOORSENSORS_H
#define OUTDOORSENSORS_H

#include "sensorinterface.h"

namespace Ui {
class OutdoorSensors;
}

class SingleOutdoorSensor;
typedef SingleOutdoorSensor *PSingleOutdoorSensor;

/**
 * UI class for the outdoor weather bricklet which can have multiple
 * outdoor sensors: currently only 4 temp sensors are supported (no weather station rain controls).
 * <p>
 * The SensorInterface is used only for the LED status, nothing else
 *
 * @brief The OutdoorSensors class
 */
class OutdoorSensors : public SensorInterface
{
    Q_OBJECT
    Ui::OutdoorSensors *ui;

    const static unsigned MAX_CHANNELS = 4;

    PSingleOutdoorSensor sensors[MAX_CHANNELS];

public:
    OutdoorSensors(QWidget *parent, const char *uid);
    ~OutdoorSensors();

    //----- VisualizationClient
    virtual void notify(const stubserver::VisibleDeviceState &state) override;
    virtual bool useAsInputSource(unsigned sensorNo) const override;
    virtual int64_t getInputState(unsigned sensorNo) const override;


public slots:
    // new sensor values for channel (updates temp + hum)
    void connected(int sensorNo);

    // new led value
    void updateLed(bool);
};

#endif // OUTDOORSENSORS_H
