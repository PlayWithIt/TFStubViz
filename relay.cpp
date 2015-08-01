
#include <QGridLayout>
#include <QLabel>
#include <QDebug>

#include <string>

#include "relay.h"
#include "ui_relay.h"

using namespace stubserver;

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


Relay::Relay(QWidget *parent, const char *type, const char *uid)
    : QWidget(parent)
    , ui(new Ui::Relay)
    , numRelays(0)
{
    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(true);

    if (type != NULL && uid != NULL)
        ui->groupBox->setTitle(QString(type) + QString(" - ") + QString(uid));
    connectTooltipTo(ui->groupBox, uid);

    if (type != NULL && strstr(type, "DUAL_")) {
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
}


/**
 * @brief Sensor::updateUi signal receiver method when a value changes
 * @param newValue
 */
void Relay::updateUi(const RelayState *rs)
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
    }
}

/**
 * @brief Sensor::notify
 * @param sensor
 */
void Relay::notify(const VisibleDeviceState &state)
{
    if (state.isDisconnected())
        return;

    try {
        const RelayState& rs = dynamic_cast<const RelayState&>(state);
        emit valueChanged(&rs);
    }
    catch (const std::exception &e) {
        qCritical("Caught exception: %s", e.what());
    }
}
