#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <QLabel>

#include "visualizationwidget.h"


namespace Ui {
class LedStrip;
}

class QPixmap;


class LedStrip : public QWidget, public VisualizationWidget
{
    Q_OBJECT

    Ui::LedStrip *ui;
    unsigned numLeds;
    const stubserver::LedStripState *ledState;

    // pixel data
    uint8_t r[stubserver::LedStripState::MAX_LEDS];
    uint8_t g[stubserver::LedStripState::MAX_LEDS];
    uint8_t b[stubserver::LedStripState::MAX_LEDS];

public:
    LedStrip(QWidget *parent, const char *uid);
    ~LedStrip();

    //----- VisualizationClient
    virtual void notify(const stubserver::VisibleDeviceState &state) override;

signals:
    void valueChanged(const stubserver::LedStripState *, unsigned to);

public slots:
    void updateUi(const stubserver::LedStripState *, unsigned to);
};

#endif // LEDSTRIP_H
