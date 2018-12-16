
#include <QFileDialog>
#include <QScrollBar>

#include <QDebug>

#include "dualbutton.h"
#include "dualsensor.h"
#include "lcd.h"
#include "lcddisplayarea.h"
#include "ledstrip.h"
#include "motionsensor.h"
#include "oled.h"
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


MainWindow::MainWindow(const char *configFile, int _port)
    : QMainWindow(0)
    , ui(new Ui::MainWindow)
    , configFileName(configFile ? configFile : "")
    , serverThread(NULL)
    , serverOK(false)
    , port(_port)
{
    ui->setupUi(this);
    ui->action_Stop->setEnabled(false);
    ui->action_Run->setEnabled(false);
    ui->centralWidget->setWidgetResizable(true);

    setWindowTitle("TF Stub-Visualization");

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
        serverThread = NULL;
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
    qDebug("serverStopped()");

    statusBar()->showMessage(tr("Server not running"));

    ui->action_Stop->setEnabled(false);
    ui->action_Run->setEnabled(true);

    QGridLayout* layout = (QGridLayout*) ui->scrollAreaWidgetContents->layout();
    if (layout) {
        // remove old widgets
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != NULL)
        {
            delete child->widget();
            delete child;
        }
    }

    serverThread->wait();
    delete serverThread;
    serverThread = NULL;
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
 * Number of columns in the GridLayout: 25 cols with 50 pixels => 1200 pixels
 */
static const int COLS_PER_ROW = 24;
static const int COLUMN_WIDTH = 50;

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

    QGridLayout* layout = (QGridLayout*) ui->scrollAreaWidgetContents->layout();
    if (!layout) {
        layout = new QGridLayout();
        layout->setVerticalSpacing(20);
        ui->scrollAreaWidgetContents->setLayout(layout);
    }

    int row = 0;
    int col = 0;
    std::list<const stubserver::SimulatedDevice*> devices = stubserver::getDevices();

    // first add the wide items
    for (auto it : devices)
    {
        VisualizationWidget *vzw = NULL;

        switch (it->getDeviceTypeId()) {
            case MULTI_TOUCH_DEVICE_IDENTIFIER: {
                TouchPad *touch = new TouchPad(this, it->getUidStr().c_str());
                calculatePositionAndAdd(row, col, layout, touch);
                touch->setLabels(it->getLabel());
                it->setVisualizationClient(*touch);
                vzw = touch;
                break;
            }

            case LCD_20X4_DEVICE_IDENTIFIER: {
                LCD *lcd = new LCD(this, it->getUidStr().c_str());
                calculatePositionAndAdd(row, col, layout, lcd);
                lcd->setLabels(it->getLabel());
                it->setVisualizationClient(*(lcd->getLCDDisplayArea()));
                vzw = lcd;
                break;
            }

            case LCD_128X64_DEVICE_IDENTIFIER: {
                OLED *w = new OLED(this, it->getUidStr().c_str());
                calculatePositionAndAdd(row, col, layout, w);
                it->setVisualizationClient(*w);
                vzw = w;
                break;
            }

            case OLED_128X64_DEVICE_IDENTIFIER:
            case OLED_64X48_DEVICE_IDENTIFIER: {
                OLED *w = new OLED(this, it->getUidStr().c_str(), it->getDeviceTypeId() == OLED_64X48_DEVICE_IDENTIFIER);
                calculatePositionAndAdd(row, col, layout, w);
                it->setVisualizationClient(*w);
                vzw = w;
                break;
            }

            case LED_STRIP_DEVICE_IDENTIFIER: {
                LedStrip *w = new LedStrip(this, it->getUidStr().c_str());
                calculatePositionAndAdd(row, col, layout, w);
                it->setVisualizationClient(*w);
                vzw = w;
                break;
            }
        }
        if (vzw)
            vzw->setStackParameter(it->getPosition(), it->getConnectedUidStr());
    }

    // add small items
    for (auto it : devices)
    {
        const char *deviceTypeName = it->getDeviceTypeName().c_str();
        const char *uidStr         = it->getUidStr().c_str();
        VisualizationWidget *vzw   = NULL;

        switch (it->getDeviceTypeId()) {

            case DUAL_RELAY_DEVICE_IDENTIFIER:
            case REMOTE_SWITCH_DEVICE_IDENTIFIER:
            case INDUSTRIAL_QUAD_RELAY_DEVICE_IDENTIFIER:
            case INDUSTRIAL_DIGITAL_OUT_4_DEVICE_IDENTIFIER: {
                Relay *relay = new Relay(this, deviceTypeName, uidStr);
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
            case CO2_DEVICE_IDENTIFIER:
            case DISTANCE_IR_DEVICE_IDENTIFIER:
            case DISTANCE_US_DEVICE_IDENTIFIER:
            case DUST_DETECTOR_DEVICE_IDENTIFIER:
            case HUMIDITY_DEVICE_IDENTIFIER:
            case LINEAR_POTI_DEVICE_IDENTIFIER:
            case LINE_DEVICE_IDENTIFIER:
            case LOAD_CELL_DEVICE_IDENTIFIER:
            case MOISTURE_DEVICE_IDENTIFIER:
            case PTC_DEVICE_IDENTIFIER:
            case SOUND_INTENSITY_DEVICE_IDENTIFIER:
            case TEMPERATURE_DEVICE_IDENTIFIER:
            case UV_LIGHT_DEVICE_IDENTIFIER: {
                Sensor *widget = new Sensor(this, deviceTypeName, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);

                if (it->getDeviceTypeId() == LOAD_CELL_DEVICE_IDENTIFIER) {
                    widget->setLedOn(true);
                }

                widget->setValueLabel(it->getLabel());
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case MASTER_DEVICE_IDENTIFIER:
            case DC_DEVICE_IDENTIFIER:
            case SERVO_DEVICE_IDENTIFIER:
            case OUTDOOR_WEATHER_DEVICE_IDENTIFIER:
            case INDUSTRIAL_DUAL_ANALOG_IN_DEVICE_IDENTIFIER:
            case VOLTAGE_CURRENT_DEVICE_IDENTIFIER: {
                DualSensor *widget = new DualSensor(this, deviceTypeName, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);
                if (it->getDeviceTypeId() == INDUSTRIAL_DUAL_ANALOG_IN_DEVICE_IDENTIFIER)
                    widget->setValueLabels("Ch 0 (mV)", "Ch 1 (mV)");
                else if (it->getDeviceTypeId() == OUTDOOR_WEATHER_DEVICE_IDENTIFIER) {
                    widget->setValueLabels("Ch 0 (°C)", "Ch 1 (°C)");
                    widget->setLedOn(true);
                }
                else {
                    widget->setValueLabels(it->getLabel(), "Current mA");
                    widget->setLedOn(true);
                }
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case TEMPERATURE_IR_DEVICE_IDENTIFIER: {
                DualSensor *widget = new DualSensor(this, deviceTypeName, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);
                widget->setValueLabels(it->getLabel(), "Ambient °C * 100");
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case ROTARY_POTI_DEVICE_IDENTIFIER: {
                Sensor *widget = new Sensor(this, deviceTypeName, uidStr, true);
                calculatePositionAndAdd(row, col, layout, widget);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case DUAL_BUTTON_DEVICE_IDENTIFIER: {
                DualButton *widget = new DualButton(this, deviceTypeName, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case HALL_EFFECT_DEVICE_IDENTIFIER:
            case TILT_DEVICE_IDENTIFIER:
            case MOTION_DETECTOR_DEVICE_IDENTIFIER: {
                MotionSensor *widget = new MotionSensor(this, deviceTypeName, uidStr);
                calculatePositionAndAdd(row, col, layout, widget);
                if (it->getDeviceTypeId() == HALL_EFFECT_DEVICE_IDENTIFIER)
                    widget->showUseCounter();
                else
                    widget->setLedOn(true);
                it->setVisualizationClient(*widget);
                vzw = widget;
                break;
            }

            case LCD_20X4_DEVICE_IDENTIFIER:
            case LCD_128X64_DEVICE_IDENTIFIER:
            case LED_STRIP_DEVICE_IDENTIFIER:
            case MULTI_TOUCH_DEVICE_IDENTIFIER:
            case OLED_128X64_DEVICE_IDENTIFIER:
            case OLED_64X48_DEVICE_IDENTIFIER:
                break;

            default:
                utils::Log() << "UI: ignored device of type " << it->getDeviceTypeId() << '(' << it->getDeviceTypeName() << ')';
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
