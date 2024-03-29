
#include <QDesktopWidget>
#include <QFileDialog>
#include <QScrollBar>

#include <QDebug>

#include "digitalin.h"
#include "dualbutton.h"
#include "dualsensor.h"
#include "lcd.h"
#include "lcddisplayarea.h"
#include "ledbutton.h"
#include "ledstrip.h"
#include "motionsensor.h"
#include "multisensor.h"
#include "outdoorsensors.h"
#include "oled.h"
#include "piezospeaker.h"
#include "relay.h"
#include "sensor.h"
#include "touchpad.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stubserver/stubserver.h>
#include <stubserver/SimulatedDevice.h>
#include <bindings/device_identifiers.h>
#include <utils/utils.h>
#include <utils/Log.h>

/**
 * Number of columns in the GridLayout:
 * 24 cols with 50 pixels => 1200 pixels
 * 30 cols with 50 pixels => 1500 pixels
 */
static int COLS_PER_ROW = 24;
static int COLUMN_WIDTH = 50;


MainWindow::MainWindow(const char *configFile, int _port)
    : QMainWindow(nullptr)
    , ui(new Ui::MainWindow)
    , configFileName(configFile ? configFile : "")
    , serverThread(nullptr)
    , serverOK(false)
    , port(_port)
{
    ui->setupUi(this);
    ui->action_Stop->setEnabled(false);
    ui->action_Run->setEnabled(false);
    ui->centralWidget->setWidgetResizable(true);

    setWindowTitle("TF Stub-Visualization");

    QDesktopWidget dw;
    if (dw.width() > 1600) {
        // set width 1420 instead of 1210
        this->setGeometry(20, 40, 1420, 1000);
        COLS_PER_ROW = 30;
    }

    if (configFileName.length() > 0)
        loadConfig();
    else
        statusBar()->showMessage(tr("Server cannot be started, no configuration selected"));
}


MainWindow::~MainWindow()
{
    if (serverThread)
    {
        on_action_Stop_triggered();
        serverThread->wait();
        delete serverThread;
        serverThread = nullptr;
    }

    delete ui;
}


void MainWindow::loadConfig()
{
    if (configFileName.length() > 0) {
        statusBar()->showMessage(tr("Using configuration: ") + configFileName);
        ui->action_Run->setEnabled(true);
    }
}


void MainWindow::serverStopped()
{
    statusBar()->showMessage(tr("Server stopped"));

    ui->action_Stop->setEnabled(false);
    ui->action_Run->setEnabled(true);

    QGridLayout* layout = static_cast<QGridLayout*>(ui->scrollAreaWidgetContents->layout());
    if (layout) {
        // remove old widgets
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr)
        {
            delete child->widget();
            delete child;
        }
    }

    serverThread->wait();
    delete serverThread;
    serverThread = nullptr;
}


void MainWindow::serverError(const QString &err)
{
    serverOK = false;
    statusBar()->showMessage(err);
}


void MainWindow::on_action_Exit_triggered()
{
    on_action_Stop_triggered();
    close();
    qApp->quit();
}


void MainWindow::on_action_Load_stub_config_triggered()
{
    configFileName = QFileDialog::getOpenFileName(this,
         tr("Open Config Properties"), ".", tr("Properties Files (*.properties)"));
    loadConfig();
}

/**
 * Calculate where to put the next widget in the GridLayout
 */
static void calculatePositionAndAdd(int &row, int &col, QGridLayout *layout, QWidget *w)
{
    int numCols = w->width() / COLUMN_WIDTH;
    if (w->width() % COLUMN_WIDTH != 0)
        ++numCols;

    if (col + numCols > COLS_PER_ROW) {
        ++row;
        col = 0;
    }
    layout->addWidget(w, row, col, 1, numCols, Qt::AlignHCenter | Qt::AlignVCenter);
    col += numCols;
}

/**
 * @brief MainWindow::on_action_Run_triggered - trigger when server should be started
 * Create new widgets and layout all the stuff
 */
void MainWindow::on_action_Run_triggered()
{
    if (serverThread)
        return;

    ui->action_Stop->setEnabled(true);
    ui->action_Run->setEnabled(false);

    serverThread = new ServerThread(configFileName, port);
    connect(serverThread, &ServerThread::error,    this, &MainWindow::serverError);
    connect(serverThread, &ServerThread::finished, this, &MainWindow::serverStopped);
    serverThread->start();
    serverOK = true;

    statusBar()->showMessage(tr("Server started"));

    utils::msleep(300);
    if (!serverOK || !serverThread)
        return;

    QGridLayout* layout = static_cast<QGridLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!layout) {
        layout = new QGridLayout();
        layout->setVerticalSpacing(20);
        ui->scrollAreaWidgetContents->setLayout(layout);
    }

    int row = 0;
    int col = 0;
    std::list<const stubserver::SimulatedDevice*> devices = stubserver::getUiDevices();

    // add the items in the order they appear: strongly recommended to use UI_UIDS in the config
    for (auto it : devices)
    {
        const char *deviceTitle = it->getTitle().c_str();
        const char *uidStr      = it->getUidStr().c_str();
        VisualizationWidget *vzw = nullptr;

        // large items
        switch (it->getDeviceTypeId()) {
        case MULTI_TOUCH_V2_DEVICE_IDENTIFIER:
        case MULTI_TOUCH_DEVICE_IDENTIFIER: {
                TouchPad *touch = new TouchPad(this, deviceTitle);
                calculatePositionAndAdd(row, col, layout, touch);
                touch->setLabels(it->getLabel());
                it->setVisualizationClient(*touch);
                vzw = touch;
                break;
            }

            case LCD_20X4_DEVICE_IDENTIFIER: {
                LCD *lcd = new LCD(this, uidStr);
                calculatePositionAndAdd(row, col, layout, lcd);
                lcd->setLabels(it->getLabel());
                it->setVisualizationClient(*(lcd->getLCDDisplayArea()));
                vzw = lcd;
                break;
            }

            case LCD_128X64_DEVICE_IDENTIFIER: {
                OLED *w = new OLED(this, deviceTitle);
                calculatePositionAndAdd(row, col, layout, w);
                it->setVisualizationClient(*w);
                vzw = w;
                break;
            }

            case OLED_128X64_DEVICE_IDENTIFIER:
            case OLED_64X48_DEVICE_IDENTIFIER: {
                OLED *w = new OLED(this, deviceTitle, it->getDeviceTypeId() == OLED_64X48_DEVICE_IDENTIFIER);
                calculatePositionAndAdd(row, col, layout, w);
                it->setVisualizationClient(*w);
                vzw = w;
                break;
            }

            case LED_STRIP_DEVICE_IDENTIFIER: {
                LedStrip *w = new LedStrip(this, uidStr);
                calculatePositionAndAdd(row, col, layout, w);
                it->setVisualizationClient(*w);
                vzw = w;
                break;
            }

            case AIR_QUALITY_DEVICE_IDENTIFIER: {
                MultiSensor *w = new MultiSensor(this, deviceTitle, 4);
                w->setValueLabel(AIR_QUALITY_IAQ, "IAQ");
                w->setValueLabel(AIR_QUALITY_TEMP, "°C * 100");
                w->setValueLabel(AIR_QUALITY_HUMIDITY, "%rH * 100");
                w->setValueLabel(AIR_QUALITY_PRESSURE, "mBar * 100");
                calculatePositionAndAdd(row, col, layout, w);
                it->setVisualizationClient(*w);
                vzw = w;
                break;
            }

            //-------------------------- add small items

            case DUAL_RELAY_DEVICE_IDENTIFIER:
            case REMOTE_SWITCH_DEVICE_IDENTIFIER:
            case INDUSTRIAL_QUAD_RELAY_DEVICE_IDENTIFIER:
            case INDUSTRIAL_QUAD_RELAY_V2_DEVICE_IDENTIFIER:
            case INDUSTRIAL_DIGITAL_OUT_4_DEVICE_IDENTIFIER:
            case INDUSTRIAL_DIGITAL_OUT_4_V2_DEVICE_IDENTIFIER: {
                Relay *relay = new Relay(this, deviceTitle, it->getDeviceTypeId() == DUAL_RELAY_DEVICE_IDENTIFIER);
                calculatePositionAndAdd(row, col, layout, relay);
                it->setVisualizationClient(*relay);
                vzw = relay;
                break;
            }

            case AMBIENT_LIGHT_DEVICE_IDENTIFIER:
            case AMBIENT_LIGHT_V2_DEVICE_IDENTIFIER:
            case ANALOG_IN_V2_DEVICE_IDENTIFIER:
            case ANALOG_OUT_DEVICE_IDENTIFIER:
            case BAROMETER_DEVICE_IDENTIFIER:
            case CO2_V2_DEVICE_IDENTIFIER:
            case DISTANCE_IR_DEVICE_IDENTIFIER:
            case DISTANCE_US_DEVICE_IDENTIFIER:
            case DUST_DETECTOR_DEVICE_IDENTIFIER:
            case HALL_EFFECT_V2_DEVICE_IDENTIFIER:
            case HUMIDITY_DEVICE_IDENTIFIER:
            case LINEAR_POTI_DEVICE_IDENTIFIER:
            case LINE_DEVICE_IDENTIFIER:
            case LOAD_CELL_DEVICE_IDENTIFIER:
            case LOAD_CELL_V2_DEVICE_IDENTIFIER:
            case MOISTURE_DEVICE_IDENTIFIER:
            case PTC_DEVICE_IDENTIFIER:
            case SOUND_INTENSITY_DEVICE_IDENTIFIER:
            case TEMPERATURE_DEVICE_IDENTIFIER:
            case TEMPERATURE_V2_DEVICE_IDENTIFIER:
            case UV_LIGHT_DEVICE_IDENTIFIER: {
                Sensor *widget = new Sensor(this, deviceTitle, false);
                calculatePositionAndAdd(row, col, layout, widget);

                if (it->getDeviceTypeId() == LOAD_CELL_DEVICE_IDENTIFIER ||
                        it->getDeviceTypeId() == LOAD_CELL_V2_DEVICE_IDENTIFIER ||
                        it->getDeviceTypeId() == HALL_EFFECT_V2_DEVICE_IDENTIFIER) {
                    widget->setLedOn(true);
                }

                widget->setValueLabel(it->getLabel());
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case HAT_DEVICE_IDENTIFIER:
            case MASTER_DEVICE_IDENTIFIER:
            case DC_DEVICE_IDENTIFIER:
            case SERVO_DEVICE_IDENTIFIER:
            case VOLTAGE_CURRENT_DEVICE_IDENTIFIER: {
                DualSensor *widget = new DualSensor(this, deviceTitle, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);
                widget->setValueLabels(it->getLabel(), "Current mA");
                widget->setLed1On(true);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case INDUSTRIAL_DUAL_ANALOG_IN_DEVICE_IDENTIFIER:
            case INDUSTRIAL_DUAL_ANALOG_IN_V2_DEVICE_IDENTIFIER: {
                DualSensor *widget = new DualSensor(this, deviceTitle, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);
                widget->setValueLabels("Ch 0 (mV)", "Ch 1 (mV)");
                if (it->getDeviceTypeId() == INDUSTRIAL_DUAL_ANALOG_IN_V2_DEVICE_IDENTIFIER) {
                    widget->setLed1On(true);
                    widget->setLed2On(true);
                }
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case OUTDOOR_WEATHER_DEVICE_IDENTIFIER: {
                OutdoorSensors *widget = new OutdoorSensors(this, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);
                widget->setLedOn(true);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case TEMPERATURE_IR_DEVICE_IDENTIFIER: {
                DualSensor *widget = new DualSensor(this, deviceTitle, "Object / Ambient");
                calculatePositionAndAdd(row, col, layout, widget);
                widget->setValueLabels(it->getLabel(), "Ambient °C * 100");
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case HUMIDITY_V2_DEVICE_IDENTIFIER: {
                DualSensor *widget = new DualSensor(this, deviceTitle, "Humidity / Temperature");
                calculatePositionAndAdd(row, col, layout, widget);
                widget->setValueLabels(it->getLabel(), "°C * 100");
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case BAROMETER_V2_DEVICE_IDENTIFIER: {
                DualSensor *widget = new DualSensor(this, deviceTitle, "AirPressure / Temperature");
                calculatePositionAndAdd(row, col, layout, widget);
                widget->setValueLabels(it->getLabel(), "°C * 100");
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case ROTARY_POTI_DEVICE_IDENTIFIER: {
                Sensor *widget = new Sensor(this, deviceTitle, true);
                calculatePositionAndAdd(row, col, layout, widget);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case PIEZO_SPEAKER_V2_DEVICE_IDENTIFIER:
            case PIEZO_SPEAKER_DEVICE_IDENTIFIER: {
                PiezoSpeaker *widget = new PiezoSpeaker(this, deviceTitle, it->getDeviceTypeId() != PIEZO_SPEAKER_DEVICE_IDENTIFIER);
                calculatePositionAndAdd(row, col, layout, widget);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case INDUSTRIAL_DIGITAL_IN_4_DEVICE_IDENTIFIER:
            case INDUSTRIAL_DIGITAL_IN_4_V2_DEVICE_IDENTIFIER: {
                DigitalIn *widget = new DigitalIn(this, deviceTitle);
                calculatePositionAndAdd(row, col, layout, widget);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case DUAL_BUTTON_DEVICE_IDENTIFIER: {
                DualButton *widget = new DualButton(this, deviceTitle);
                calculatePositionAndAdd(row, col, layout, widget);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case RGB_LED_V2_DEVICE_IDENTIFIER:
            case RGB_LED_BUTTON_DEVICE_IDENTIFIER: {
                LedButton *widget = new LedButton(this, it->getDeviceTypeId() == RGB_LED_V2_DEVICE_IDENTIFIER, deviceTitle);
                calculatePositionAndAdd(row, col, layout, widget);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case HALL_EFFECT_DEVICE_IDENTIFIER:
            case TILT_DEVICE_IDENTIFIER:
            case MOTION_DETECTOR_DEVICE_IDENTIFIER:
            case MOTION_DETECTOR_V2_DEVICE_IDENTIFIER: {
                MotionSensor *widget = new MotionSensor(this, deviceTitle);
                calculatePositionAndAdd(row, col, layout, widget);

                if (it->getDeviceTypeId() == MOTION_DETECTOR_V2_DEVICE_IDENTIFIER)
                    widget->showAllLeds();
                if (it->getDeviceTypeId() == HALL_EFFECT_DEVICE_IDENTIFIER)
                    widget->showUseCounter();
                else
                    widget->setLedOn(true);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            default:
                utils::Log() << "UI: ignored device of type " << it->getDeviceTypeName() << " (uid=" << it->getUidStr() << ')';
        }
        if (vzw)
            vzw->setStackParameter(it->getPosition(), it->getConnectedUidStr());
    }
    ui->scrollAreaWidgetContents->setMinimumHeight(315 * (row+1));
}


void MainWindow::on_action_Stop_triggered()
{
    serverOK = false;
    if (serverThread)
        serverThread->doStop();
}
