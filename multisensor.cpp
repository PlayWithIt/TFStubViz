#include <QHBoxLayout>
#include "multisensor.h"


MultiSensor::MultiSensor(QWidget *parent, const char *title, unsigned sensorCount)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    bool isAirQuality = sensorCount == 4;

    layout->setSpacing(0);
    layout->setContentsMargins(QMargins(0,0,0,0));

    sensors.push_back(new Sensor(this, title, false));
    sensors.push_back(new Sensor(this, isAirQuality ? "Temp" : "", false));
    for (unsigned s = 2; s < sensorCount; ++s)
        sensors.push_back( new Sensor(this, isAirQuality ? ( s == 2 ? "Humidity" : "Pressure" ): "", false) );

    unsigned w = 3;
    for (auto it : sensors) {
        layout->addWidget( it );
        w += it->width();
    }

    // Workaround to set min / max for AIR_QUALITY
    if (isAirQuality)
    {
        sensors[AIR_QUALITY_IAQ]->setMinMax(0, 500);
        sensors[AIR_QUALITY_TEMP]->setMinMax(-5000, 5000);
        sensors[AIR_QUALITY_HUMIDITY]->setMinMax(0, 100 * 100);
        sensors[AIR_QUALITY_PRESSURE]->setMinMax(0, 100 * 1400);
    }

    this->setObjectName("MultiSensor");
    this->setLayout(layout);
    this->setFixedHeight(sensors[0]->height() + 2);
    this->setFixedWidth(w);

    setStyleSheet("QWidget#DualSensor { border-radius: 5px; border: 1px solid black; border: 2px groove gray; }");
}

MultiSensor::~MultiSensor()
{
    for (auto it : sensors)
        delete it;
}


void MultiSensor::setValueLabel(unsigned sensor, const std::string &label)
{
    sensors[sensor]->setValueLabel(label);
}

void MultiSensor::setStackParameter(char position, const std::string &parent)
{
    for (auto it : sensors)
        it->setStackParameter(position, parent);
}

/**
 * Receives the notification when the sensor value changes.
 */
void MultiSensor::notify(const stubserver::VisibleDeviceState &state)
{
    if (state.getChangeCode() == stubserver::VisibleDeviceState::DISCONNECT)
        return;
    if (state.getChangeCode() == stubserver::VisibleDeviceState::CONNECTED)
    {
        // qDebug("Notify for all sensors with state %d", state.getChangeCode());
        if (sensors.size() == 4) {
            // AirQuality: init all 4 sensors with initial value + min/max
            const stubserver::SensorState& sensorState = dynamic_cast<const stubserver::SensorState&>(state);

            emit sensors[AIR_QUALITY_IAQ]->valueChanged(sensorState.getSensorValue1());
            emit sensors[AIR_QUALITY_TEMP]->valueChanged(sensorState.getSensorValue2());
            emit sensors[AIR_QUALITY_HUMIDITY]->valueChanged(sensorState.getSensorValue3());
            emit sensors[AIR_QUALITY_PRESSURE]->valueChanged(sensorState.getSensorValue4());
        }
    }
    else {
        unsigned sensorNo = state.getInternalSensorNo();
        if (sensorNo < sensors.size()) {
            // qDebug("Notify sensor %d with state change", sensorNo);
            sensors[sensorNo]->notify(state);
        }
    }
}

/**
 * A simulated sensor needs simulated values which can come from a ValueProvider
 * or from the visualisation, choose the source by this function. The default
 * implementation returns false: use ValueProvider.
 */
bool MultiSensor::useAsInputSource(unsigned sensorNo) const
{
    bool res = false;
    if (sensorNo < sensors.size()) {
        res = sensors[sensorNo]->useAsInputSource(sensorNo);
    }
    return res;
}

/**
 * Return a device specific state (0 in default impl).
 */
int64_t MultiSensor::getInputState(unsigned sensorNo) const {
    if (sensorNo < sensors.size())
        return sensors[sensorNo]->getInputState(sensorNo);
    return 0;
}
