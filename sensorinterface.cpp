
#include <QDebug>
#include "sensorinterface.h"

SensorInterface::SensorInterface(QWidget *p, QWidget *_statusLED)
  : QWidget(p)
  , StatusLed(_statusLED)
  , checkBox(NULL)
  , currentValue(0)
  , counter(0)
  , min(0)
  , max(100)
  , minMaxSet(false)
  , manualControl(false)
{
}


/**
 * Start to use manual control for the values.
 */
void SensorInterface::activateManualControl()
{
    if (!checkBox)
        return;
    manualControl = true;
    checkBox->setEnabled(true);
    checkBox->setChecked(true);
}

/**
 * @brief Sensor::manualSliderChange
 */
void SensorInterface::checkBoxClicked(int v)
{
    manualControl = v != 0;
    if (checkBox)
        checkBox->setEnabled(manualControl);
}

/**
 * Receives the notification when the sensor value changes.
 */
void SensorInterface::notify(const stubserver::VisibleDeviceState &sensor)
{
    if (sensor.isDisconnected())
        return;

    try {
        const stubserver::SensorState& state = dynamic_cast<const stubserver::SensorState&>(sensor);

        if (state.getChangeCode() == stubserver::SensorState::LED_CHANGE)
        {
            // just change the LED status, nothing else
            emit ledChanged(state.isStatusLedOn());
            return;
        }

        // change sensor value
        int newValue = state.getSensorValue();
        if (!minMaxSet) {
            this->min = state.getMin();
            this->max = state.getMax();
        }
        counter = state.getCounter();
        emit valueChanged(newValue);
    }
    catch (const std::exception &e) {
        qCritical("Caught exception: %s", e.what());
    }
}

/**
 * A simulated sensor needs simulated values which can come from a ValueProvider
 * or from the visualisation, choose the source by this function. The default
 * implementation returns false: use ValueProvider.
 */
bool SensorInterface::useAsInputSource(unsigned) const {
    return manualControl;
}

/**
 * Return a device specific state (0 in default impl).
 */
int64_t SensorInterface::getInputState(unsigned) const {
    return currentValue;
}
