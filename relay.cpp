
#include <QGridLayout>
#include <QLabel>
#include <QDebug>

#include <string>

#include "relay.h"
#include "ui_relay.h"

/**
 * Load a pixmap from ressources.
 */
static QPixmap loadPixmap(const char *p)
{
    std::string path(":/rc_all/images/");
    std::string full(path + p);

    QPixmap pixmap(full.c_str());
    return pixmap.scaledToWidth(180);
}


Relay::Relay(QWidget *parent, const char *title, bool dual)
    : QWidget(parent)
    , ui(new Ui::Relay)
    , numRelays(0)
{
    memset(statusLED, 0, sizeof(statusLED));

    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(true);

    ui->groupBox->setTitle(title);
    connectTooltipTo(ui->groupBox);

    if (dual) {
        imgOn  = new QPixmap(loadPixmap("dual_on.png"));
        imgOff = new QPixmap(loadPixmap("dual_off.png"));
    }
    else {
        imgOn  = new QPixmap(loadPixmap("single_on.png"));
        imgOff = new QPixmap(loadPixmap("single_off.png"));
    }

    connect(this, &Relay::valueChanged, this, &Relay::updateUi);
}

Relay::~Relay()
{
    delete ui;
    delete imgOn;
    delete imgOff;

    for (int i = 0; i < numRelays; ++i)
        delete statusLED[i];
}


/**
 * @brief Sensor::updateUi signal receiver method when a value changes
 * @param newValue
 */
void Relay::updateUi(const stubserver::RelayState *rs)
{
    // e.g. the RemoteSwitch has dynamic switches
    int count = rs->getNumSwitches();
    if (count == 0)
        return;

    if (numRelays < count)
    {
        // first init or dynamically add a switch (RemoteSwitch code)
        QGridLayout *layout;

        if (numRelays == 0)
        {
            layout = new QGridLayout();
            layout->setHorizontalSpacing(20);
        }
        else
            layout = (QGridLayout*) ui->scrollAreaWidgetContents->layout();

        int height = 0;
        for (int i = numRelays; i < count; ++i)
        {
            QPixmap *img = (rs->isOn(i) ? imgOn : imgOff);
            int minHeight = img->height() + 10;

            QLabel *lbl = new QLabel();
            //lbl->setPixmap(*img);
            lbl->setMinimumHeight(minHeight);
            layout->setRowMinimumHeight(i, minHeight);
            height += minHeight;
            layout->addWidget(lbl, i, 0);

            imgWidgets[i] = lbl;

            lbl = new QLabel();
            lbl->setText(rs->getLabel(i).c_str());
            lbl->setMinimumHeight(img->height() + 10);
            layout->addWidget(lbl, i, 1);

            if (rs->getLedConfig(i) != stubserver::StatusLedConfig::LED_HIDDEN) {
                QWidget *led = new QWidget();
                led->setMinimumWidth(12);
                led->setMinimumHeight(12);
                led->setMaximumWidth(12);
                led->setMaximumHeight(12);
                statusLED[i] = new StatusLed(led);
                layout->addWidget(led, i, 2);
            }
        }

        if (numRelays == 0)
        {
            ui->scrollAreaWidgetContents->setMinimumHeight(height);
            ui->scrollAreaWidgetContents->setLayout(layout);
        }
        numRelays = count;
    }

    // now show the relay state
    for (int i = 0; i < count; ++i)
    {
        QPixmap *img = rs->isOn(i) ? imgOn : imgOff;
        QLabel *lbl = imgWidgets[i];
        lbl->setPixmap(*img);
        if (statusLED[i])
            statusLED[i]->setLedOn(rs->isLedOn(i));
    }
}

/**
 * @brief Sensor::notify
 * @param sensor
 */
void Relay::notify(const stubserver::VisibleDeviceState &state)
{
    if (state.isDisconnected())
        return;

    try {
        const stubserver::RelayState& rs = dynamic_cast<const stubserver::RelayState&>(state);
        emit valueChanged(&rs);
    }
    catch (const std::exception &e) {
        qCritical("Caught exception: %s", e.what());
    }
}
