#ifndef LCDDISPLAYAREA_H
#define LCDDISPLAYAREA_H

#include <QPainter>
#include <QPen>

#include <stubserver/CharMask.h>
#include <utils/Mutexes.h>
#include "buttonwidget.h"

/**
 * @brief The RenderArea class used for simple drawing.
 */
class LCDDisplayArea : public ButtonWidget
{
    Q_OBJECT

    const unsigned dotSize;          // size of one dot of a char (in screen pixels)
    const unsigned dot6Size;         // == 6 * dotSize (hoizontal size per char)
    const unsigned dot9Size;         // == 9 * dotSize (vertical size per char)
    bool           backlight;
    QPen           whitePen;
    QPen           grayPen;

    stubserver::ScreenFont     lcdFont;
    const stubserver::LcdState *lcdState;
    std::mutex myMutex;

    void initCharMasks();
    void showBacklight();

    /**
     * Calculate X position for a dot in a char
     */
    unsigned getX(unsigned charX, unsigned dot);

    /**
     * Calculate Y position for a dot in a char
     */
    unsigned getY(unsigned charY, unsigned dot);

    void drawChar(char actCh, unsigned col, unsigned line, QPainter &painter);

public:
    LCDDisplayArea(QWidget *parent);
    virtual ~LCDDisplayArea();

    virtual void notify(const stubserver::VisibleDeviceState &hint) override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

signals:

public slots:
};

#endif // LCDDISPLAYAREA_H
