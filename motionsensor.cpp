#include "motionsensor.h"
#include "ui_motionsensor.h"

MotionSensor::MotionSensor(QWidget *parent, const char *type, const char *uid)
    : SensorInterface(parent)
    , ui(new Ui::MotionSensor)
    , useCounter(false)
{
    ui->setupUi(this);
    ui->check->setDisabled(true);
    setCheckBox(ui->check);

    ui->groupBox->setTitle(QString(type) + QString(" - ") + QString(uid));
    connectTooltipTo(ui->groupBox, uid);

    statusLED = ui->statusLED;

    // Connect the button and the checkbox
    connect(this,       &MotionSensor::valueChanged, this, &MotionSensor::updateUi);
    connect(ui->button, &QPushButton::pressed,       this, &SensorInterface::activateManualControl);
    connect(ui->button, &QPushButton::released,      this, &MotionSensor::toggleState);
    connect(ui->check,  &QCheckBox::stateChanged,    this, &SensorInterface::checkBoxClicked);
}

MotionSensor::~MotionSensor()
{
    delete ui;
}

/**
 * Change state of the motion sensor and update color.
 */
void MotionSensor::toggleState()
{
    updateUi(currentValue == 0 ? 1 : 0);
}

/**
 * @brief MotionSensor::updateUi signal receiver method when a value changes
 * @param newValue
 */
void MotionSensor::updateUi(int newValue)
{
    currentValue = newValue;
    if (!minMaxSet)
        minMaxSet = true;

    if (useCounter)
        ui->label->setText(QString::number(counter));

    if (max == 1 && currentValue) {
        ui->button->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
        ui->button->setText("MOTION");
    }
    else if (max == 2 && currentValue == 1) {
        ui->button->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
        ui->button->setText("TILT");
    }
    else if (max == 2 && currentValue == 2) {
        ui->button->setStyleSheet("QPushButton {background-color: #AA5555; color: black;}");
        ui->button->setText("SHAKE");
    }
    else {
        ui->button->setStyleSheet("QPushButton {background-color: #909090; color: black;}");
        ui->button->setText("IDLE");
    }
}
