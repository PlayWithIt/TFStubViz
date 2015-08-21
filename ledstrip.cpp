
#include <QGridLayout>
#include <QDebug>

#include <string>

#include "ledstrip.h"
#include "ui_ledstrip.h"

using namespace stubserver;


LedStrip::LedStrip(QWidget *parent, const char *uid)
    : QWidget(parent)
    , ui(new Ui::LedStrip)
    , numLeds(0)
    , ledState(NULL)
{
    ui->setupUi(this);
    ui->groupBox->setTitle(QString("LedStrip - ") + QString(uid));
    connectTooltipTo(ui->groupBox, uid);

    connect(this, &LedStrip::valueChanged, this, &LedStrip::updateUi);
}

LedStrip::~LedStrip()
{
    delete ui;
}


#define LEDS_PER_ROW 40

/**
 * The LED normally don't use color 0..255 has already has the max brightness at 127
 * and is still dark at 20. To equalize this a little bit on the simulation use a
 * limited color range.
 */
inline unsigned getColor(unsigned c) {
    unsigned ret = 50 + 2 * c;
    if (ret > 255)
        return 255;
    return ret;
}

/**
 * @brief Sensor::updateUi signal receiver method when a value changes
 * @param newValue
 */
void LedStrip::updateUi(const LedStripState *strip, unsigned to)
{
    ledState = strip;
    if (strip == NULL)
        return;

    QGridLayout *layout;
    if (numLeds == 0) {
        numLeds = strip->getNumLeds();

        if (numLeds > 0) {
            char buf[100];
            sprintf(buf, "%d", numLeds);
            ui->groupBox->setTitle(ui->groupBox->title() + " - " + QString(buf) + QString(" LEDs in the strip"));
        }

        layout = new QGridLayout();
        layout->setHorizontalSpacing(3);
        layout->setVerticalSpacing(10);

        for (int i = 0; i < LEDS_PER_ROW; ++i)
            layout->setColumnMinimumWidth(i, 15);
        for (int i = 0; i < 8; ++i)
            layout->setRowMinimumHeight(i, 15);
        ui->groupBox->setLayout(layout);

        for (unsigned x = 0; x < numLeds; ++x) {
            QWidget *lbl = new QWidget();
            lbl->setMinimumHeight(15);
            lbl->setMaximumHeight(30);
            lbl->setMinimumWidth(15);
            layout->addWidget(lbl, x / LEDS_PER_ROW, x % LEDS_PER_ROW);
        }
    }
    else {
        layout = dynamic_cast<QGridLayout *>(ui->groupBox->layout());
        if (layout == NULL) {
            qDebug("LedStrip - layout is NULL!");
            return;
        }
    }

    unsigned from = 0;
    while (from < to) {
        QWidget *lbl = layout->itemAtPosition(from / LEDS_PER_ROW, from % LEDS_PER_ROW)->widget();
        if (!lbl) {
            qDebug("label is NULL at %d", from);
        }
        else {
            char style[64];
            sprintf(style, "background:rgb(%u,%u,%u);",
                    getColor(r[from]),
                    getColor(g[from]),
                    getColor(b[from]));

            //qDebug(style);
            lbl->setStyleSheet(style);
        }
        ++from;
    }
    // qDebug("Updated %d LEDs", to);
}

/**
 * Receives the notification when the sensor value changes.
 */
void LedStrip::notify(const stubserver::VisibleDeviceState &sensor)
{
    if (sensor.isDisconnected()) {
        ledState = NULL;
        return;
    }

    try {
        const stubserver::LedStripState& state = dynamic_cast<const stubserver::LedStripState&>(sensor);
        if (state.getFrameData(r, g, b) > 0)
            emit valueChanged(&state, state.getFrameLength());
    }
    catch (const std::exception &e) {
        qCritical("Caught exception: %s", e.what());
    }
}
