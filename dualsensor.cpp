#include <QHBoxLayout>
#include "dualsensor.h"


DualSensor::DualSensor(QWidget *parent, const char *type, const char *uid)
    : QWidget(parent)
    , sensor0( new Sensor(this, type, NULL) )
    , sensor1( new Sensor(this, uid, NULL) )
{
    QHBoxLayout *layout = new QHBoxLayout;

    layout->setSpacing(0);
    layout->setContentsMargins(QMargins(0,0,0,0));
    layout->addWidget(sensor0);
    layout->addWidget(sensor1);

    this->setObjectName("DualSensor");
    this->setLayout(layout);
    this->setFixedHeight(sensor0->height() + 2);
    this->setFixedWidth(sensor0->width() + sensor1->width() + 3);

    setStyleSheet("QWidget#DualSensor { border-radius: 5px; border: 1px solid black; border: 2px groove gray; }");
}

DualSensor::~DualSensor()
{
    delete sensor0;
    delete sensor1;
}


void DualSensor::setValueLabels(const std::string &left, const std::string &right)
{
    sensor0->setValueLabel(left);
    sensor1->setValueLabel(right);
}

void DualSensor::setStackParameter(char position, const std::string &parent)
{
    sensor0->setStackParameter(position, parent);
    sensor1->setStackParameter(position, parent);
}

/**
 * Receives the notification when the sensor value changes.
 */
void DualSensor::notify(const stubserver::VisibleDeviceState &sensor)
{
    // qDebug("Notify for sensor %d", sensor.getInternalSensorNo());
    if (sensor.getInternalSensorNo() == 0)
        sensor0->notify(sensor);
    else
        sensor1->notify(sensor);
}

/**
 * A simulated sensor needs simulated values which can come from a ValueProvider
 * or from the visualisation, choose the source by this function. The default
 * implementation returns false: use ValueProvider.
 */
bool DualSensor::useAsInputSource(unsigned sensorNo) const {
    if (sensorNo == 0)
        return sensor0->useAsInputSource(sensorNo);
    return sensor1->useAsInputSource(sensorNo);
}

/**
 * Return a device specific state (0 in default impl).
 */
int64_t DualSensor::getInputState(unsigned sensorNo) const {
    if (sensorNo == 0)
        return sensor0->getInputState(sensorNo);
    return sensor1->getInputState(sensorNo);
}
