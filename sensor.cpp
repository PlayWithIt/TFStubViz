
#include <QDebug>

#include "sensor.h"
#include "ui_sensor.h"
#include "ui_poti.h"


using namespace stubserver;


Sensor::Sensor(QWidget *parent, const char *type, const char *uid, bool rotary)
    : SensorInterface(parent)
    , uiS(rotary ? NULL : new Ui::Sensor())
    , uiP(rotary ? new Ui::Potentiometer() : NULL)
    , statusLED(NULL)
{
    if (rotary) {
        uiP->setupUi(this);
        uiP->groupBox->setTitle(QString(type) + QString(" - ") + QString(uid));
        setCheckBox(uiP->check);
        connectTooltipTo(uiP->groupBox, uid);

        sensor   = uiP->sensor;
        labelMin = uiP->labelMin;
        labelMax = uiP->labelMax;
        label    = uiP->label;
    }
    else {
        uiS->setupUi(this);
        uiS->groupBox->setTitle(QString(type) + QString(" - ") + QString(uid));
        setCheckBox(uiS->check);
        connectTooltipTo(uiS->groupBox, uid);

        sensor   = uiS->sensor;
        labelMin = uiS->labelMin;
        labelMax = uiS->labelMax;
        label    = uiS->label;
        statusLED= uiS->statusLED;
    }

    connect(this,     &Sensor::valueChanged,             this, &Sensor::updateUi);
    connect(this,     &Sensor::ledChanged,               this, &Sensor::updateLed);
    connect(sensor,   &QAbstractSlider::sliderPressed,   this, &SensorInterface::activateManualControl);
    connect(sensor,   &QAbstractSlider::sliderMoved,     this, &Sensor::updateUi);
    connect(sensor,   &QAbstractSlider::actionTriggered, this, &Sensor::actionTriggered);
    connect(checkBox, &QCheckBox::stateChanged,          this, &SensorInterface::checkBoxClicked);
}

Sensor::~Sensor()
{
    setCheckBox(NULL);
    delete uiP;
    delete uiS;
}

/**
 * Sets the additional label for the value type.
 * @param label - label string
 */
void Sensor::setValueLabel(const std::string &label) const
{
    if (label.length() > 0 && uiS)
        uiS->valueTypeLabel->setText(label.c_str());
}

/**
 * Slider changes via keyboard.
 */
void Sensor::actionTriggered(int action)
{
    int newValue = currentValue;

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
        newValue = min;
        break;
    case QAbstractSlider::SliderToMaximum:
        newValue = max;
        break;
    case QAbstractSlider::SliderNoAction:
    case QAbstractSlider::SliderMove:
        return;
    }

    if (newValue > max)
        newValue = max;
    if (newValue < min)
        newValue = min;

    currentValue = newValue;
    sensor->setValue(newValue);
    label->setText(QString::number(newValue));
}

/**
 * @brief setLedOn turn the status LED on or OFF (for Bricks).
 * @param on if true, LED is on
 */
void Sensor::setLedOn(bool on)
{
    if (statusLED == NULL)
        return;

    if (on)
        statusLED->setStyleSheet("background-color:blue;");
    else
        statusLED->setStyleSheet("background-color:gray;");
}

/**
 * @brief Sensor::updateUi signal receiver method when a value changes
 * @param newValue
 */
void Sensor::updateUi(int newValue)
{
    currentValue = newValue;
    if (!minMaxSet)
    {
        labelMin->setText(QString::number(min));
        labelMax->setText(QString::number(max));
        label->setText(QString::number(newValue));
        sensor->setRange(min, max);
        sensor->setPageStep((max - min) / 80);
        minMaxSet = true;
    }

    label->setText(QString::number(newValue));

    // this method was called due to manual change?
    if (!manualControl)
        sensor->setValue(newValue);
}

/**
 * @brief Sensor::updateLed signal receiver method when the led state changes
 * @param ledValue
 */
void Sensor::updateLed(bool ledValue)
{
    setLedOn(ledValue);
}
