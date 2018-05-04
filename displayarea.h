#ifndef DISPLAYAREA_H
#define DISPLAYAREA_H

#include <QPainter>
#include <QPen>

#include <utils/Mutexes.h>
#include <stubserver/VisualizationClient.h>

/**
 * The DisplayArea is a field to draw text (LCD) and/or single pixels (OLED).
 * It holds the pixel size and screen size, but not the LCD buttons.
 */
class DisplayArea
{
    Q_OBJECT

    const unsigned dotSize;          // size of one dot of a char (in screen pixels)
    const unsigned dot6Size;         // == 6 * dotSize (hoizontal size per char)
    const unsigned dot9Size;         // == 9 * dotSize (vertical size per char)
    bool           backlight;        // normal or color inverted / with backlight?
    QPen           whitePen;         // normal foreground color
    QPen           grayPen;          // inverted foreground color

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

    /**
     * Draw a single char at the given position.
     */
    void drawChar(char actCh, unsigned col, unsigned line, QPainter &painter);

public:
    DisplayArea(QWidget *parent);
    virtual ~DisplayArea();

    virtual void notify(const stubserver::VisibleDeviceState &hint) override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // DISPLAYAREA_H
