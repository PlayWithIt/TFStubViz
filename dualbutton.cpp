
#include <stubserver/VisualizationClient.h>

#include "dualbutton.h"
#include "ui_dualbutton.h"

DualButton::DualButton(QWidget *parent, const char *type, const char *uid)
    : SensorInterface(parent)
    , led_l(NULL)
    , led_r(NULL)
    , ui(new Ui::DualButton)
{
    ui->setupUi(this);
    setCheckBox(ui->check);

    ui->groupBox->setTitle(QString(type) + QString(" - ") + QString(uid));
    connectTooltipTo(ui->groupBox, uid);

    led_l = new StatusLed(ui->statusLED_l);
    led_r = new StatusLed(ui->statusLED_r);

    // Connect the button and the checkbox
    connect(this,         &DualButton::valueChanged, this, &DualButton::updateUi);
    connect(ui->button_l, &QPushButton::pressed,     this, [this]{ buttonDown(0); });
    connect(ui->button_l, &QPushButton::released,    this, [this]{ buttonUp(0); });
    connect(ui->button_r, &QPushButton::pressed,     this, [this]{ buttonDown(1); });
    connect(ui->button_r, &QPushButton::released,    this, [this]{ buttonUp(1); });
    connect(checkBox,     &QCheckBox::stateChanged,  this, &SensorInterface::checkBoxClicked);
}

DualButton::~DualButton()
{
    setCheckBox(NULL);
    delete led_l;
    delete led_r;
    delete ui;
}

/**
 * Receives the notification when the sensor value changes.
 */
void DualButton::notify(const stubserver::VisibleDeviceState &sensor)
{
    if (sensor.isDisconnected())
        return;

    try {
        const stubserver::DualButtonState& state = dynamic_cast<const stubserver::DualButtonState&>(sensor);

        // change sensor value
        int newValue = state.getButtonStates();
        led_l->setLedOn(state.isLedOn_l(), false);
        led_r->setLedOn(state.isLedOn_r(), false);

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
void DualButton::buttonDown(int no)
{
    if (no == 0)
        currentValue |= 1;
    else
        currentValue |= 2;
    activateManualControl();
    emit valueChanged(currentValue);
}

void DualButton::buttonUp(int no)
{
    if (no == 0)
        currentValue ^= 1;
    else
        currentValue ^= 2;
    emit valueChanged(currentValue);
}

/**
 * @brief DualButton::updateUi signal receiver method when a button state changes
 * @param newValue
 */
void DualButton::updateUi(int newValue)
{
    currentValue = newValue;

    if ((currentValue & 1) != 0)
        ui->button_l->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
    else
        ui->button_l->setStyleSheet("QPushButton {background-color: #909090; color: black;}");

    if ((currentValue & 2) != 0)
        ui->button_r->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
    else
        ui->button_r->setStyleSheet("QPushButton {background-color: #909090; color: black;}");

    // update LED status
    led_l->setLedOn(led_l->isLedOn());
    led_r->setLedOn(led_r->isLedOn());

    // TODO: this method was called due to manual change?
    //if (!manualControl)
    //    sensor->setValue(newValue);
}
