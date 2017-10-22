#ifndef OLED_H
#define OLED_H

#include <QPainter>
#include <QPen>
#include <QWidget>

#include <utils/Mutexes.h>
#include "visualizationwidget.h"

namespace Ui {
class OledForm;
}

/**
 * Class that handles OLED with 128x64 and 64x48 pixels.
 */
class OLED : public QWidget, public VisualizationWidget
{
    Q_OBJECT

    QPen          whitePen;
    QPen          grayPen;
    Ui::OledForm *ui;
    unsigned      lines, cols;   // virtual pixels on screen

    const stubserver::OledState *oledState;
    std::mutex myMutex;

    void clear();

public:
    /**
     * Init UI, if 'small' is true, then 64x48 pixels are used, otherwise 128x64.
     */
    explicit OLED(QWidget *parent, const char *uid, bool small);

    virtual void notify(const stubserver::VisibleDeviceState &hint) override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

};

#endif // OLED_H
