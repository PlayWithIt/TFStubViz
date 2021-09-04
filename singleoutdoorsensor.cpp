#include "singleoutdoorsensor.h"

#define MIN_TEMP    -300    // -30°
#define MAX_TEMP    500     // 50°
#define MIN_HUM     0
#define MAX_HUM     100

/**
 * Init UI elements, still not connected to simulator.
 *
 * @param parent
 * @param checkBox
 * @param sTemp
 * @param sHum
 * @param lTemp
 * @param lHum
 * @param lId
 */
SingleOutdoorSensor::SingleOutdoorSensor(QWidget *parent, QCheckBox *checkBox, QSlider *sTemp, QSlider *sHum,
                                         QLabel *lTemp, QLabel *lHum, QLabel *lId)
  : QWidget(parent)
  , id(-1)
  , currentTemp(100)  // 10°
  , currentHumidity(40)
  , manualControl(false)
  , checkManual(checkBox)
  , sliderTemp(sTemp)
  , sliderHum(sHum)
  , labelTemp(lTemp)
  , labelHum(lHum)
  , labelId(lId)
{
    sliderTemp->setRange(MIN_TEMP, MAX_TEMP);
    sliderHum->setRange(MIN_HUM, MAX_HUM);
    checkManual->setEnabled(true);

    connect(this, &SingleOutdoorSensor::newValues, this, &SingleOutdoorSensor::updateSensors);

    connect(sliderTemp,  &QAbstractSlider::sliderPressed,   this, &SingleOutdoorSensor::activateManualControl);
    connect(sliderHum,   &QAbstractSlider::sliderPressed,   this, &SingleOutdoorSensor::activateManualControl);
    connect(sliderTemp,  &QAbstractSlider::actionTriggered, this, &SingleOutdoorSensor::tempActionTriggered);
    connect(sliderHum,   &QAbstractSlider::actionTriggered, this, &SingleOutdoorSensor::humActionTriggered);
    connect(sliderTemp,  &QAbstractSlider::sliderMoved,     this, &SingleOutdoorSensor::tempSliderMoved);
    connect(sliderHum,   &QAbstractSlider::sliderMoved,     this, &SingleOutdoorSensor::humSliderMoved);
    connect(checkManual, &QCheckBox::stateChanged,          this, &SingleOutdoorSensor::toggleManualControl);

    //TODO: click on 'manual' to enable auto mode.
}


/**
 * @brief OutdoorSensors::updateUi signal receiver method when a sensor was
 *        pressed and manual control is enabled.
 * @param sensorNo 0 .. 3
 */
void SingleOutdoorSensor::activateManualControl()
{
    if (!manualControl) {
        manualControl = true;
        checkManual->setChecked(true);
    }
}


/**
 * SingleOutdoorSensor::toggleManualControl change state of manual control
 * for the sliders.
 */
void SingleOutdoorSensor::toggleManualControl(int v)
{
    manualControl = v != 0;
}


/**
 * Temperature slider changes via keyboard.
 */
void SingleOutdoorSensor::tempActionTriggered(int action)
{
    int newValue = currentTemp;

    switch (action) {
    case QAbstractSlider::SliderPageStepAdd:
        newValue += 10;
        break;
    case QAbstractSlider::SliderPageStepSub:
        newValue -= 10;
        break;
    case QAbstractSlider::SliderSingleStepAdd:
        newValue += 1;
        break;
    case QAbstractSlider::SliderSingleStepSub:
        newValue -= 1;
        break;
    case QAbstractSlider::SliderToMinimum:
        newValue = MIN_TEMP;
        break;
    case QAbstractSlider::SliderToMaximum:
        newValue = MAX_TEMP;
        break;
    case QAbstractSlider::SliderNoAction:
    case QAbstractSlider::SliderMove:
        return;
    }

    if (newValue > MAX_TEMP)
        newValue = MAX_TEMP;
    if (newValue < MIN_TEMP)
        newValue = MIN_TEMP;

    currentTemp = newValue;
    sliderTemp->setValue(newValue);
    labelTemp->setText(QString::number(0.1 * static_cast<double>(currentTemp), 'f', 1));
}


/**
 * Humidity slider changes via keyboard.
 */
void SingleOutdoorSensor::humActionTriggered(int action)
{
    int newValue = currentHumidity;

    switch (action) {
    case QAbstractSlider::SliderPageStepAdd:
        newValue += 5;
        break;
    case QAbstractSlider::SliderPageStepSub:
        newValue -= 5;
        break;
    case QAbstractSlider::SliderSingleStepAdd:
        newValue += 1;
        break;
    case QAbstractSlider::SliderSingleStepSub:
        newValue -= 1;
        break;
    case QAbstractSlider::SliderToMinimum:
        newValue = MIN_HUM;
        break;
    case QAbstractSlider::SliderToMaximum:
        newValue = MAX_HUM;
        break;
    case QAbstractSlider::SliderNoAction:
    case QAbstractSlider::SliderMove:
        return;
    }

    if (newValue > MAX_HUM)
        newValue = MAX_HUM;
    if (newValue < MIN_HUM)
        newValue = MIN_HUM;

    currentHumidity = newValue;
    sliderHum->setValue(newValue);
    labelHum->setText(QString::number(currentHumidity));
}


/**
 * @brief triggered when the temperature slider is moved via mouse
 */
void SingleOutdoorSensor::tempSliderMoved(int value)
{
    // printf("updateUI %d\n", sensorNo);

    currentTemp = value;
    sliderTemp->setValue(currentTemp);
    labelTemp->setText(QString::number(0.1 * static_cast<double>(currentTemp), 'f', 1));
}


/**
 * @brief triggered when the temperature slider is moved via mouse
 */
void SingleOutdoorSensor::humSliderMoved(int value)
{
    // printf("updateUI %d\n", sensorNo);

    currentHumidity = value;
    sliderHum->setValue(currentHumidity);
    labelHum->setText(QString::number(currentHumidity));
}


/**
 * @brief triggered when both values are changed
 */
void SingleOutdoorSensor::updateSensors(int temp, int hum)
{
    tempSliderMoved(temp);
    humSliderMoved(hum);
}


/**
 * @brief OutdoorSensors::updateUi signal receiver to update all labels and values
 * @param sensorNo 0 .. 3
 */
void SingleOutdoorSensor::updateAll()
{
    // printf("updateUI %d\n", sensorNo);

    // update ID and all values
    labelId->setText(QString("ID ") + QString::number(id));

    sliderTemp->setValue(currentTemp);
    sliderHum->setValue(currentHumidity);
    labelTemp->setText(QString::number(0.1 * static_cast<double>(currentTemp), 'f', 1));
    labelHum->setText(QString::number(currentHumidity));
}
