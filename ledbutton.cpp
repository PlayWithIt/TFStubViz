
#include <stubserver/VisualizationClient.h>

#include "ledbutton.h"
#include "ui_ledbutton.h"

LedButton::LedButton(QWidget *parent, const char *title)
    : SensorInterface(parent)
    , led_l(nullptr)
    , ui(new Ui::LedButton)
    , red(0)
    , green(0)
    , blue(0)
{
    ui->setupUi(this);
    setCheckBox(ui->check);

    ui->groupBox->setTitle(title);
    connectTooltipTo(ui->groupBox);

    led_l = new StatusLed(ui->statusLED);

    // Connect the button and the checkbox
    connect(this,       &LedButton::valueChanged,  this, &LedButton::updateUi);
    connect(ui->button, &QPushButton::pressed,     this, [this]{ buttonDown(); });
    connect(ui->button, &QPushButton::released,    this, [this]{ buttonUp(); });
    connect(checkBox,   &QCheckBox::stateChanged,  this, &SensorInterface::checkBoxClicked);
}

LedButton::~LedButton()
{
    setCheckBox(nullptr);
    delete led_l;
    delete ui;
}

/**
 * Receives the notification when the sensor value changes.
 */
void LedButton::notify(const stubserver::VisibleDeviceState &sensor)
{
    if (sensor.isDisconnected())
        return;

    try {
        const stubserver::DualButtonState& state = dynamic_cast<const stubserver::DualButtonState&>(sensor);

        // change sensor value
        int newValue = state.getButtonStates();
        led_l->setLedOn(state.isStatusLedOn(), false);
        red   = state.getRed();
        green = state.getGreen();
        blue  = state.getBlue();

        emit valueChanged(newValue);
    }
    catch (const std::exception &e) {
        qCritical("Caught exception: %s", e.what());
    }
}

/**
 * Change the button state and indirectly also the LED state.
 * 0 for LEFT, 1 for RIGHT button
 */
void LedButton::buttonDown()
{
    currentValue = 1;
    activateManualControl();
    emit valueChanged(currentValue);
}

void LedButton::buttonUp()
{
    currentValue = 0;
    emit valueChanged(currentValue);
}

/**
 * @brief LedButton::updateUi signal receiver method when a button state changes
 * @param newValue
 */
void LedButton::updateUi(int newValue)
{
    char color[128];
    const char *text;

    currentValue = newValue;

    if (currentValue) {
        if (red + green + blue < 100)
            text = "red";
        else
            text = "yellow";
    }
    else {
        if (red + green + blue < 100)
            text = "white";
        else
            text = "black";
    }

    sprintf(color, "QPushButton {background-color: #%02X%02X%02X; color: %s;}",
            red, green, blue, text);

    ui->button->setStyleSheet(color);

    // update LED status
    led_l->setLedOn(led_l->isLedOn(), true);

    // TODO: this method was called due to manual change?
    //if (!manualControl)
    //    sensor->setValue(newValue);
}
