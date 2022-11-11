
#include <utils/utils.h>

#include "piezospeaker.h"
#include "ui_piezo.h"


PiezoSpeaker::PiezoSpeaker(QWidget *parent, const char *title, bool isV2)
    : SensorInterface(parent)
    , uiSpeaker(new Ui::PiezoSpeaker())
{
    uiSpeaker->setupUi(this);
    uiSpeaker->groupBox->setTitle(title);
    connectTooltipTo(uiSpeaker->groupBox);

    statusLED = uiSpeaker->statusLED;
    if (isV2)
        setLedOn(true);

    connect(this, &PiezoSpeaker::valueChanged, this, &PiezoSpeaker::updateUi);
    connect(this, &PiezoSpeaker::ledChanged,   this, &PiezoSpeaker::updateLed);
}

PiezoSpeaker::~PiezoSpeaker()
{
    delete uiSpeaker;
}


/**
 * @brief Sensor::updateUi signal receiver method when a value changes
 * @param newValue
 */
void PiezoSpeaker::updateUi(int newValue)
{
    char buffer[32];
    currentValue = newValue;
    uiSpeaker->lcdNumber->display(utils::itoa(buffer, newValue));
}

/**
 * @brief PiezoSpeaker::updateLed signal receiver method when the led state changes
 * @param ledValue
 */
void PiezoSpeaker::updateLed(bool ledValue)
{
//  printf("PiezoSpeaker::setLedOn(%d), led = %p\n", ledValue, statusLED);
    setLedOn(ledValue);
}
