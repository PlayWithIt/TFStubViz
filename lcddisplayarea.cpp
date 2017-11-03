
#include <QDebug>

#include "lcddisplayarea.h"

using namespace stubserver;

/**
 * Size of the internal LCD border
 */
static const unsigned BORDER_SIZE = 10;

/**
 * Dots per char in horizontal direction
 */
static const unsigned COLS_PER_CHAR = CharMask::NUM_COLS - 1;

/**
 * Dots per char in vertical direction: there are 9 dots,
 * but normally only 8 are used and the 9th is already a separator
 * which optionally can be used.
 */
static const unsigned ROWS_PER_CHAR = CharMask::NUM_LINES;


/**
 * Create the LCD with a blue background.
 */
LCDDisplayArea::LCDDisplayArea(QWidget *parent)
    : ButtonWidget(parent)
    , dotSize(5)
    , dot6Size(CharMask::NUM_COLS * dotSize)
    , dot9Size(CharMask::NUM_LINES * dotSize)
    , backlight(false)
    , whitePen(Qt::white, dotSize - 1)
    , grayPen(Qt::gray, dotSize - 1)
    , lcdFont(false)
    , lcdState(NULL)
    , myMutex()
{
    showBacklight();
}

/**
 * Cleanup datasctructures.
 */
LCDDisplayArea::~LCDDisplayArea()
{
}

void LCDDisplayArea::showBacklight()
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, backlight ? Qt::blue : Qt::darkBlue);
    setAutoFillBackground(true);
    setPalette(Pal);
}

/**
 * Calculate X position for a dot in a char
 */
unsigned LCDDisplayArea::getX(unsigned charX, unsigned dot)
{
    return BORDER_SIZE + (charX * dot6Size) + (dot * dotSize);
}

/**
 * Calculate Y position for a dot in a char
 */
unsigned LCDDisplayArea::getY(unsigned charY, unsigned dot)
{
    return BORDER_SIZE + (charY * dot9Size) + (dot * dotSize);
}

/**
 * Draw one single char on the LCD.
 * @param actCh
 * @param painter
 */
void LCDDisplayArea::drawChar(char actCh, unsigned col, unsigned line, QPainter &painter)
{
    unsigned numPoints = 0;
    QPoint points[CharMask::NUM_COLS * CharMask::NUM_LINES];

   // PCharMask m = chars[actCh < 0 ? actCh + 255 : actCh];
    PCharMask m = lcdFont.get(actCh & 255);
    for (unsigned x = 0; x < COLS_PER_CHAR; ++x)
        for (unsigned y = 0; y < ROWS_PER_CHAR; ++y)
        {
            if (m->isOn(x, y)) {
                points[numPoints] = QPoint(getX(col, x), getY(line, y));
                ++numPoints;
            }
        }

    painter.drawPoints(points, numPoints);
}

/**
 * Redraw the screen with the current text content.
 */
void LCDDisplayArea::paintEvent(QPaintEvent * /* event */)
{
    utils::MutexLock lock(myMutex);
    if (lcdState == NULL)
        return;

    unsigned lines = lcdState->getLines();
    unsigned cols  = lcdState->getCols();

    if (backlight != lcdState->isBacklightOn())
    {
        backlight = lcdState->isBacklightOn();
        showBacklight();
    }

    QPainter painter(this);
    painter.setPen(backlight ? whitePen : grayPen);

    for (unsigned line = 0; line < lines; ++line)
    {
        const std::string& text = lcdState->getLine(line);
        for (unsigned actCol = 0; actCol < cols; ++actCol)
        {
            char actCh = text[actCol];

            // do not really print a space
            if (actCh == ' ')
                continue;

            drawChar(actCh, actCol, line, painter);
        }
    }

    if (lcdState->isCursorVisible())
    {
        drawChar(255, lcdState->getCursorX(), lcdState->getCursorY(), painter);
    }
}

void LCDDisplayArea::notify(const VisibleDeviceState &hint)
{
    //qDebug() << "LCDDisplayArea::notify " << hint.getChangeCode();

    if (hint.getChangeCode() == VisibleDeviceState::DISCONNECT)
        lcdState = NULL;
    else {
        lcdState = dynamic_cast<const LcdState*>(&hint);
        if (lcdState && hint.getChangeCode() == LcdState::CUSTOM_CHAR)
        {
            unsigned line = lcdState->getChangedLine();
            if (line < 8)
            {
                // custom chars are defined by 0..7 and index by char 8..15
                lcdFont.set(line + 8, new CharMask(lcdState->getCustomerChar(line)));
            }
        }
        else
            update();
    }
}
