#ifndef OLED_H
#define OLED_H

#include <QPainter>
#include <QPen>
#include <QWidget>
#include <QMouseEvent>

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
    QPen          bluePen;
    QPen          grayPen;
    Ui::OledForm *ui;
    unsigned      lines, cols;   // virtual pixels on screen
    bool          isLcd, mouseIsDown;

    const stubserver::DisplayState *displayState;
    std::mutex myMutex;

    void clear(bool inverted);

    /**
     * Calculate the screen pixel coordinate out of the QWidget event position
     * @return true if the event was within the screen.
     */
    bool getScreenPixel(QPoint &point, unsigned &col, unsigned &line) const;

public:
    /**
     * Init UI, if 'small' is true, then 64x48 pixels are used, otherwise 128x64.
     */
    explicit OLED(QWidget *parent, const char *uid, bool small);

    /**
     * Used to create the LCD only !
     */
    explicit OLED(QWidget *parent, const char *uid);
    ~OLED();

    void notify(const stubserver::VisibleDeviceState &hint) override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

};

#endif // OLED_H
