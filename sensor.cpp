
#include <QDebug>

#include "sensor.h"
#include "ui_sensor.h"
#include "ui_poti.h"


using namespace stubserver;


Sensor::Sensor(QWidget *parent, const char *title, bool rotary)
    : SensorInterface(parent)
    , uiS(rotary ? nullptr : new Ui::Sensor())
    , uiP(rotary ? new Ui::Potentiometer() : nullptr)
{
    QGroupBox *grp;
    if (rotary) {
        uiP->setupUi(this);
        setCheckBox(uiP->check);

        grp      = uiP->groupBox;
        sensor   = uiP->sensor;
        labelMin = uiP->labelMin;
        labelMax = uiP->labelMax;
        label    = uiP->label;
    }
    else {
        uiS->setupUi(this);
        setCheckBox(uiS->check);
        connectTooltipTo(uiS->groupBox);

        grp      = uiS->groupBox;
        sensor   = uiS->sensor;
        labelMin = uiS->labelMin;
        labelMax = uiS->labelMax;
        label    = uiS->label;
        statusLED= uiS->statusLED;
    }

    grp->setTitle(title);

    connect(this,     &Sensor::valueChanged,             this, &Sensor::updateUi);
    connect(this,     &Sensor::ledChanged,               this, &Sensor::updateLed);
    connect(sensor,   &QAbstractSlider::sliderPressed,   this, &SensorInterface::activateManualControl);
    connect(sensor,   &QAbstractSlider::sliderMoved,     this, &Sensor::sliderMoved);
    connect(sensor,   &QAbstractSlider::actionTriggered, this, &Sensor::actionTriggered);
    connect(checkBox, &QCheckBox::stateChanged,          this, &SensorInterface::checkBoxClicked);
}

Sensor::~Sensor()
{
    setCheckBox(nullptr);
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

    if (newValue + zeroPoint > max)
        newValue = max;
    if (newValue + zeroPoint < min)
        newValue = min;

    // printf("actionTriggered %d %d, current %d\n", newValue, zeroPoint, currentValue);

    currentValue = newValue;
    sensor->setValue(newValue + zeroPoint);
    label->setText(QString::number(newValue));
}

// new sensor value manually (other zero point handling)
void Sensor::sliderMoved(int v)
{
    // printf("sliderMoved %d %d, current %d\n", v, zeroPoint, currentValue);
    updateUi(v - zeroPoint);
}

/**
 * @brief Sensor::updateUi signal receiver method when a value changes
 * @param newValue
 */
void Sensor::updateUi(int newValue)
{
    // printf("updateUi %d %d, current %d\n", newValue, zeroPoint, currentValue);
    currentValue = newValue;
    if (!minMaxSet)
    {
        labelMin->setText(QString::number(min));
        labelMax->setText(QString::number(max));
        sensor->setRange(min, max);
        sensor->setPageStep((max - min) / 80);
        minMaxSet = true;
    }

    label->setText(QString::number(newValue));

    // this method was called due to manual change?
    if (!manualControl)
        sensor->setValue(newValue + zeroPoint);
}

/**
 * @brief Sensor::updateLed signal receiver method when the led state changes
 * @param ledValue
 */
void Sensor::updateLed(bool ledValue)
{
    setLedOn(ledValue);
}
