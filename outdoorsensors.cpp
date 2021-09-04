
#include <QDebug>

#include "outdoorsensors.h"
#include "singleoutdoorsensor.h"
#include "ui_outdoorsensors.h"


using namespace stubserver;

/**
 * Init the UI behavior, link the objects.
 * @brief OutdoorSensors::OutdoorSensors
 * @param title
 * @param uid
 */
OutdoorSensors::OutdoorSensors(QWidget *parent, const char *uid)
    : SensorInterface(parent)
    , ui(new Ui::OutdoorSensors())
{
    ui->setupUi(this);
    ui->groupBox_2->setTitle(uid);
    connectTooltipTo(ui->groupBox);

    // connect the status LED, we have only one
    statusLED = ui->statusLED;

    // default init for the sensors values
    sensors[0] = new SingleOutdoorSensor(this, ui->check, ui->sensorTemp, ui->sensorHum, ui->labelTemp, ui->labelRH, ui->idLabel);
    sensors[1] = new SingleOutdoorSensor(this, ui->check_2, ui->sensorTemp_2, ui->sensorHum_2, ui->labelTemp_2, ui->labelRH_2, ui->idLabel_2);
    sensors[2] = new SingleOutdoorSensor(this, ui->check_3, ui->sensorTemp_3, ui->sensorHum_3, ui->labelTemp_3, ui->labelRH_3, ui->idLabel_3);
    sensors[3] = new SingleOutdoorSensor(this, ui->check_4, ui->sensorTemp_4, ui->sensorHum_4, ui->labelTemp_4, ui->labelRH_4, ui->idLabel_4);

    connect(this, &OutdoorSensors::valueChanged, this, &OutdoorSensors::connected);
    connect(this, &OutdoorSensors::ledChanged,   this, &OutdoorSensors::updateLed);
}


/**
 * @brief cleanup resources
 */
OutdoorSensors::~OutdoorSensors()
{
    delete sensors[0];
    delete sensors[1];
    delete sensors[2];
    delete sensors[3];
    delete ui;
}


/**
 * Receives the notification when the sensor value changes.
 */
void OutdoorSensors::notify(const VisibleDeviceState &sensor)
{
    if (sensor.isDisconnected())
        return;

    try {
        const OutdoorWeatherState& state = dynamic_cast<const OutdoorWeatherState&>(sensor);

        if (state.getChangeCode() == SensorState::LED_CHANGE)
        {
            // just change the LED status, nothing else
            emit ledChanged(state.isStatusLedOn());
            return;
        }
        if (state.getChangeCode() == SensorState::CONNECTED)
        {
            // change the ID label of the outdoor temp sensors
            unsigned last = state.getNumSensors();
            for (unsigned i = 0; i < last && i < MAX_CHANNELS; ++i) {
                const OutdoorWeatherState::BaseData owData = state.getState(i);
                sensors[i]->connectSensor(owData.getId(), owData.getTemperatoure(), owData.getHumidity());
            }
            emit valueChanged(-1);
            return;
        }

        unsigned no = state.getInternalSensorNo();
        const OutdoorWeatherState::BaseData owData = state.getCurrentState();

        if (no < MAX_CHANNELS) {
            // printf("value changed %u %d %u\n", no, owData.getTemperatoure(), owData.getHumidity());
            emit sensors[no]->newValues(owData.getTemperatoure(), owData.getHumidity());
        }
        else {
            qCritical("OutdoorSensors::notify - invalid sensor no %u", no);
        }

    }
    catch (const std::exception &e) {
        qCritical("OutdoorSensors::notify - caught exception: %s", e.what());
    }
}

/**
 * A simulated sensor needs simulated values which can come from a ValueProvider
 * or from the visualisation, choose the source by this function. The default
 * implementation returns false: use ValueProvider.
 */
bool OutdoorSensors::useAsInputSource(unsigned channel) const
{
    if (channel >= MAX_CHANNELS)
        return false;
    return sensors[channel]->isManualControl();
}

/**
 * Return the temp + humidity for channel 0 .. 4.
 */
int64_t OutdoorSensors::getInputState(unsigned channel) const
{
    if (channel >= MAX_CHANNELS)
        return 0;

    // put two integers in one result value
    union {
        struct {
            int temp;
            int hum;
        };
        int64_t result;
    } data;

    data.temp = sensors[channel]->getCurrentTemp();
    data.hum  = sensors[channel]->getCurrentHumidity();

    return data.result;
}

/**
 * @brief OutdoorSensors::updateLed signal receiver method when the led state changes
 * @param ledValue
 */
void OutdoorSensors::updateLed(bool ledValue)
{
    setLedOn(ledValue);
}

/**
 * @brief OutdoorSensors::updateUi signal receiver method when a sensor changes
 */
void OutdoorSensors::connected(int)
{
    // printf("updateUI %d\n", sensorNo);
    sensors[0]->updateAll();
    sensors[1]->updateAll();
    sensors[2]->updateAll();
    sensors[3]->updateAll();
}
