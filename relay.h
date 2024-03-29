#ifndef RELAY_H
#define RELAY_H

#include <QLabel>

#include "statusled.h"
#include "visualizationwidget.h"


namespace Ui {
class Relay;
}

class QPixmap;


class Relay : public QWidget, public VisualizationWidget
{
    Q_OBJECT

public:
    Relay(QWidget *parent, const char *title, bool dual);
    ~Relay();

    //----- VisualizationClient
    virtual void notify(const stubserver::VisibleDeviceState &state) override;

signals:
    void valueChanged(const stubserver::RelayState *rs);

public slots:
    void updateUi(const stubserver::RelayState *rs);

private:
    Ui::Relay *ui;
    QPixmap   *imgOn;
    QPixmap   *imgOff;
    QLabel    *imgWidgets[16];
    StatusLed *statusLED[16];

    int numRelays;
};

#endif // RELAY_H
