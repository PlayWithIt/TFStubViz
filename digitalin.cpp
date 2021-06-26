
#include <stubserver/VisualizationClient.h>

#include "digitalin.h"
#include "ui_digitalin.h"

DigitalIn::DigitalIn(QWidget *parent, const char *type, const char *uid)
    : SensorInterface(parent)
    , led0(NULL)
    , led1(NULL)
    , led2(NULL)
    , led3(NULL)
    , ui(new Ui::DigitalIn)
{
    ui->setupUi(this);
    setCheckBox(ui->check);

    ui->groupBox->setTitle(QString(type) + QString(" - ") + QString(uid));
    connectTooltipTo(ui->groupBox, uid);

    led0 = new StatusLed(ui->statusLED0);
    led1 = new StatusLed(ui->statusLED1);
    led2 = new StatusLed(ui->statusLED2);
    led3 = new StatusLed(ui->statusLED3);

    // Connect the button and the checkbox
    connect(this,        &DigitalIn::valueChanged, this, &DigitalIn::updateUi);
    connect(ui->button0, &QPushButton::clicked,    this, [this]{ buttonClick(1); });
    connect(ui->button1, &QPushButton::clicked,    this, [this]{ buttonClick(2); });
    connect(ui->button2, &QPushButton::clicked,    this, [this]{ buttonClick(4); });
    connect(ui->button3, &QPushButton::clicked,    this, [this]{ buttonClick(8); });
    connect(checkBox,    &QCheckBox::stateChanged, this, &SensorInterface::checkBoxClicked);
}

DigitalIn::~DigitalIn()
{
    setCheckBox(NULL);
    delete led0;
    delete led1;
    delete led2;
    delete led3;
    delete ui;
}

/**
 * Receives the notification when the sensor value changes.
 */
void DigitalIn::notify(const stubserver::VisibleDeviceState &sensor)
{
    if (sensor.isDisconnected())
        return;

    try {
        const stubserver::RelayState& state = dynamic_cast<const stubserver::RelayState&>(sensor);

        // change sensor value
        int newValue = state.getSwitchStates();
        led0->setLedOn(state.isLedOn(0), false);
        led1->setLedOn(state.isLedOn(1), false);
        led2->setLedOn(state.isLedOn(2), false);
        led3->setLedOn(state.isLedOn(3), false);

        emit valueChanged(newValue);
    }
    catch (const std::exception &e) {
        qCritical("Caught exception: %s", e.what());
    }
}

/**
 * Change the button state: invert the bit flag on each click.
 */
void DigitalIn::buttonClick(int no)
{
    if (0 == (currentValue & no)) {
        currentValue |= no;           // set flag
    }
    else {
        currentValue ^= no;           // clear flag
    }
    activateManualControl();
    emit valueChanged(currentValue);
}

/**
 * @brief DigitalIn::updateUi signal receiver method when a button state changes
 * @param newValue
 */
void DigitalIn::updateUi(int newValue)
{
    currentValue = newValue;

    if ((currentValue & 1) != 0)
        ui->button0->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
    else
        ui->button0->setStyleSheet("QPushButton {background-color: #909090; color: black;}");

    if ((currentValue & 2) != 0)
        ui->button1->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
    else
        ui->button1->setStyleSheet("QPushButton {background-color: #909090; color: black;}");

    if ((currentValue & 4) != 0)
        ui->button2->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
    else
        ui->button2->setStyleSheet("QPushButton {background-color: #909090; color: black;}");

    if ((currentValue & 8) != 0)
        ui->button3->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
    else
        ui->button3->setStyleSheet("QPushButton {background-color: #909090; color: black;}");

    // update LED status
    led0->setLedOn(led0->isLedOn(), true);
    led1->setLedOn(led1->isLedOn(), true);
    led2->setLedOn(led2->isLedOn(), true);
    led3->setLedOn(led3->isLedOn(), true);

    // TODO: this method was called due to manual change?
    //if (!manualControl)
    //    sensor->setValue(newValue);
}
