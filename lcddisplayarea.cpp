
#include <QDebug>

#include "lcddisplayarea.h"
#include "CharMask.h"

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


static PCharMask chars[256];
static unsigned  objectCount = 0;


/**
 * Create the LCD render are with blue background.
 */
LCDDisplayArea::LCDDisplayArea(QWidget *parent)
    : ButtonWidget(parent)
    , dotSize(5)
    , dot6Size(CharMask::NUM_COLS * dotSize)
    , dot9Size(CharMask::NUM_LINES * dotSize)
    , backlight(false)
    , whitePen(Qt::white, dotSize - 1)
    , grayPen(Qt::gray, dotSize - 1)
    , lcdState(NULL)
    , myMutex()
{
    showBacklight();

    if (++objectCount == 1)
        initCharMasks();
}

/**
 * Cleanup datasctructures.
 */
LCDDisplayArea::~LCDDisplayArea()
{
    if (--objectCount == 0)
    {
        for (int i = 0; i < 256; ++i)
            delete(chars[i]);
        bzero(chars, sizeof(chars));
    }
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
    PCharMask m = chars[actCh & 255];
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
    utils::MutexLock lock(myMutex);
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
                delete chars[line + 8];
                chars[line + 8] = new CharMask(lcdState->getCustomerChar(line));
            }
        }
        else
            update();
    }
}


/**
 * much, much code to set the pixels of each char in the pixel font.
 */
void LCDDisplayArea::initCharMasks()
{
    bzero(chars, sizeof(chars));

    chars['!'] = new CharMask("  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "     ",
                              "  X  " );

    chars['"'] = new CharMask(" X X ",
                              " X X ",
                              " X X ",
                              "     ",
                              "     ",
                              "     ",
                              "     " );

    chars['#'] = new CharMask(" X X ",
                              " X X ",
                              "XXXXX",
                              " X X ",
                              "XXXXX",
                              " X X ",
                              " X X " );

    chars['$'] = new CharMask("  X  ",
                              " XXXX",
                              "X X  ",
                              " XXX ",
                              "  X X",
                              "XXXX ",
                              "  X  " );

    chars['%'] = new CharMask("XX   ",
                              "XX  X",
                              "   X ",
                              "  X  ",
                              " X   ",
                              "X  XX",
                              "   XX");

    chars['&'] = new CharMask(" XX  ",
                              "X  X ",
                              "X X  ",
                              " X   ",
                              "X X X",
                              "X  X ",
                              " XX X");

    chars['\'']= new CharMask(" XX  ",
                              "  X  ",
                              " X   ",
                              "     ",
                              "     ",
                              "     ",
                              "     ");

    chars['('] = new CharMask("   X ",
                              "  X  ",
                              " X   ",
                              " X   ",
                              " X   ",
                              "  X  ",
                              "   X " );

    chars[')'] = new CharMask(" X   ",
                              "  X  ",
                              "   X ",
                              "   X ",
                              "   X ",
                              "  X  ",
                              " X   " );

    chars['*'] = new CharMask("     ",
                              "  X  ",
                              "x X x",
                              " XXX ",
                              "x X x",
                              "  X  ",
                              "     ");

    chars['+'] = new CharMask("     ",
                              "  X  ",
                              "  X  ",
                              "XXXXX",
                              "  X  ",
                              "  X  ",
                              "     ");

    chars[','] = new CharMask("     ",
                              "     ",
                              "     ",
                              "     ",
                              " XX  ",
                              "  X  ",
                              " X   ");

    chars['-'] = new CharMask("     ",
                              "     ",
                              "     ",
                              "XXXXX",
                              "     ",
                              "     ",
                              "     ");

    chars['.'] = new CharMask("     ",
                              "     ",
                              "     ",
                              "     ",
                              "     ",
                              " XX  ",
                              " XX  ");

    chars['/'] = new CharMask("     ",
                              "    X",
                              "   X ",
                              "  X  ",
                              " X   ",
                              "X    ",
                              "     ");

    chars['0'] = new CharMask(" XXX ",
                              "X   X",
                              "X  XX",
                              "X X X",
                              "XX  X",
                              "X   X",
                              " XXX ");

    chars['1'] = new CharMask("  X  ",
                              " XX  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              " XXX " );

    chars['2'] = new CharMask(" XXX ",
                              "X   X",
                              "    X",
                              "   X ",
                              "  X  ",
                              " X   ",
                              "XXXXX");

    chars['3'] = new CharMask("XXXXX",
                              "   X ",
                              "  X  ",
                              "   X ",
                              "    X",
                              "X   X",
                              " XXX " );

    chars['4'] = new CharMask("   X ",
                              "  XX ",
                              " X X ",
                              "X  X ",
                              "XXXXX",
                              "   X ",
                              "   X ");

    chars['5'] = new CharMask("XXXXX",
                              "X    ",
                              "XXXX ",
                              "    X",
                              "    X",
                              "X   X",
                              " XXX " );

    chars['6'] = new CharMask("  XX ",
                              " X   ",
                              "X    ",
                              "XXXX ",
                              "X   X",
                              "X   X",
                              " XXX " );

    chars['7'] = new CharMask("XXXXX",
                              "    X",
                              "   X ",
                              "  X  ",
                              " X   ",
                              " X   ",
                              " X   ");

    chars['8'] = new CharMask(" XXX ",
                              "X   X",
                              "X   X",
                              " XXX ",
                              "X   X",
                              "X   X",
                              " XXX " );

    chars['9'] = new CharMask(" XXX ",
                              "X   X",
                              "X   X",
                              " XXXX",
                              "    X",
                              "   X ",
                              " XX  ");

    chars[':'] = new CharMask("     ",
                              " XX  ",
                              " XX  ",
                              "     ",
                              " XX  ",
                              " XX  ",
                              "     ");

    chars['A'] = new CharMask(" XXX ",
                              "X   X",
                              "X   X",
                              "XXXXX",
                              "X   X",
                              "X   X",
                              "X   X" );

    chars['B'] = new CharMask("XXXX ",
                              "X   X",
                              "X   X",
                              "XXXX ",
                              "X   X",
                              "X   X",
                              "XXXX " );

    chars['C'] = new CharMask(" XXX ",
                              "X   X",
                              "X    ",
                              "X    ",
                              "X    ",
                              "X   X",
                              " XXX " );

    chars['D'] = new CharMask("XXXX ",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              "XXXX " );

    chars['E'] = new CharMask("XXXXX",
                              "X    ",
                              "X    ",
                              "XXX  ",
                              "X    ",
                              "X    ",
                              "XXXXX" );

    chars['F'] = new CharMask("XXXXX",
                              "X    ",
                              "X    ",
                              "XXX  ",
                              "X    ",
                              "X    ",
                              "X    " );

    chars['G'] = new CharMask(" XXX ",
                              "X   X",
                              "X    ",
                              "X XXX",
                              "X   X",
                              "X   X",
                              " XXXX" );

    chars['H'] = new CharMask("X   X",
                              "X   X",
                              "X   X",
                              "XXXXX",
                              "X   X",
                              "X   X",
                              "X   X" );

    chars['I'] = new CharMask(" XXX ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              " XXX " );

    chars['J'] = new CharMask("  XXX",
                              "   X ",
                              "   X ",
                              "   X ",
                              "   X ",
                              "X  X ",
                              " XX  " );

    chars['K'] = new CharMask("X   X",
                              "X  X ",
                              "X X  ",
                              "XX   ",
                              "X X  ",
                              "X  X ",
                              "X   X" );

    chars['L'] = new CharMask("X    ",
                              "X    ",
                              "X    ",
                              "X    ",
                              "X    ",
                              "X    ",
                              "XXXXX" );

    chars['M'] = new CharMask("X   X",
                              "XX XX",
                              "X X X",
                              "X X X",
                              "X   X",
                              "X   X",
                              "X   X" );

    chars['N'] = new CharMask("X   X",
                              "X   X",
                              "XX  X",
                              "X X X",
                              "X  XX",
                              "X   X",
                              "X   X" );

    chars['O'] = new CharMask(" XXX ",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              " XXX " );

    chars['P'] = new CharMask("XXXX ",
                              "X   X",
                              "X   X",
                              "XXXX ",
                              "X    ",
                              "X    ",
                              "X    " );

    chars['Q'] = new CharMask(" XXX ",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X X X",
                              " XXX ",
                              "   X ");

    chars['R'] = new CharMask("XXXX ",
                              "X   X",
                              "X   X",
                              "XXXX ",
                              "X   X",
                              "X   X",
                              "X   X" );

    chars['S'] = new CharMask(" XXXX",
                              "X    ",
                              "X    ",
                              " XXX ",
                              "    X",
                              "    X",
                              "XXXX ");

    chars['T'] = new CharMask("XXXXX",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  " );

    chars['U'] = new CharMask("X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              " XXX ");

    chars['V'] = new CharMask("X   X",
                              "X   X",
                              "X   X",
                              "X   X",
                              " X X ",
                              " X X ",
                              "  X  " );

    chars['W'] = new CharMask("X   X",
                              "X   X",
                              "X   X",
                              "X X X",
                              "X X X",
                              "X X X",
                              " X X ");

    chars['X'] = new CharMask("X   X",
                              "X   X",
                              " X X ",
                              "  X  ",
                              " X X ",
                              "X   X",
                              "X   X" );

    chars['Y'] = new CharMask("X   X",
                              "X   X",
                              " X X ",
                              " X X ",
                              "  X  ",
                              "  X  ",
                              "  X  ");

    chars['Z'] = new CharMask("XXXXX",
                              "   X ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              " X   ",
                              "XXXXX" );

    chars['['] = new CharMask(" XXX ",
                              " X   ",
                              " X   ",
                              " X   ",
                              " X   ",
                              " X   ",
                              " XXX ");

    chars['\\']= new CharMask("X X X",
                              " XXX ",
                              "XXXXX",
                              "  X  ",
                              "XXXXX",
                              "  X  ",
                              "  X  ");

    chars[']'] = new CharMask(" XXX ",
                              "   X ",
                              "   X ",
                              "   X ",
                              "   X ",
                              "   X ",
                              " XXX ");

    chars['_'] = new CharMask("     ",
                              "     ",
                              "     ",
                              "     ",
                              "     ",
                              "     ",
                              "XXXXX");

    chars['='] = new CharMask("     ",
                              "     ",
                              "XXXXX",
                              "     ",
                              "XXXXX",
                              "     ",
                              "     ");

    chars['?'] = new CharMask(" XXX ",
                              "X   X",
                              "X   X",
                              "   X ",
                              "  X  ",
                              "     ",
                              "  X  " );

    // the hex value is that one from the LCD charset!
    chars[0xA1] = new CharMask("   ",
                               "   ",
                               "   ",
                               "   ",
                               "XXX",
                               "X X",
                               "XXX",
                               "   ");

    chars[0xDF] = new CharMask("XXX",
                               "X X",
                               "XXX",
                               "   ",
                               "   ",
                               "   ",
                               "   ");
    // ä
    chars[0xE1] = new CharMask(" X X ",
                               "     ",
                               " XXX ",
                               "    X",
                               " XXXX",
                               "X   X",
                               " XXXX" );
    // ß
    chars[0xE2] = new CharMask("     ",
                               " XXX ",
                               "X   X",
                               "XXXX ",
                               "X   X",
                               "XXXX ",
                               "X    ",
                               "X    " );

    chars[0xEB] = new CharMask("X X",
                               " X ",
                               "X X",
                               "   ",
                               "   ",
                               "   ",
                               "   ");

    chars['a'] = new CharMask("     ",
                              "     ",
                              " XXX ",
                              "    X",
                              " XXXX",
                              "X   X",
                              " XXXX" );

    chars['b'] = new CharMask("X    ",
                              "X    ",
                              "X XX ",
                              "XX  X",
                              "X   X",
                              "X   X",
                              "XXXX " );

    chars['c'] = new CharMask("     ",
                              "     ",
                              " XXX ",
                              "X    ",
                              "X    ",
                              "X   X",
                              " XXX " );

    chars['d'] = new CharMask("    X",
                              "    X",
                              " XX X",
                              "X  XX",
                              "X   X",
                              "X   X",
                              " XXXX" );

    chars['e'] = new CharMask("     ",
                              "     ",
                              " XXX ",
                              "X   X",
                              "XXXXX",
                              "X    ",
                              " XXX " );

    chars['f'] = new CharMask("  XX ",
                              " X  X",
                              " X   ",
                              "XXX  ",
                              " X   ",
                              " X   ",
                              " X   " );

    chars['g'] = new CharMask("     ",
                              "     ",
                              " XXX ",
                              "X   X",
                              "X   X",
                              " XXXX",
                              "    X",
                              " XXX " );

    chars['h'] = new CharMask("X    ",
                              "X    ",
                              "X XX ",
                              "XX  X",
                              "X   X",
                              "X   X",
                              "X   X" );

    chars['i'] = new CharMask("  x  ",
                              "     ",
                              " xX  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              " XXX " );

    chars['j'] = new CharMask("   X ",
                              "     ",
                              "  xx ",
                              "   X ",
                              "   X ",
                              "X  X ",
                              " XX  " );

    chars['k'] = new CharMask("X    ",
                              "X    ",
                              "X  X ",
                              "X X  ",
                              "XX   ",
                              "X X  ",
                              "X  X " );

    chars['l'] = new CharMask(" XX  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              " XXX " );

    chars['m'] = new CharMask("     ",
                              "     ",
                              "XX X ",
                              "X X X",
                              "X X X",
                              "X   X",
                              "X   X" );

    chars['n'] = new CharMask("     ",
                              "     ",
                              "X XX ",
                              "XX  X",
                              "X   X",
                              "X   X",
                              "X   X" );

    chars['o'] = new CharMask("     ",
                              "     ",
                              " XXX ",
                              "X   X",
                              "X   X",
                              "X   X",
                              " XXX " );

    // ö
    chars[0xEF] = new CharMask(" X X ",
                               "     ",
                               " XXX ",
                               "X   X",
                               "X   X",
                               "X   X",
                               " XXX " );

    chars['p'] = new CharMask("     ",
                              "     ",
                              "XXXX ",
                              "X   X",
                              "XXXX ",
                              "X    ",
                              "X    " );

    chars['q'] = new CharMask("     ",
                              "     ",
                              " XXXX",
                              "X   X",
                              " XXXX",
                              "    X",
                              "    X");

    chars['r'] = new CharMask("     ",
                              "     ",
                              "X XX ",
                              "XX  X",
                              "X    ",
                              "X    ",
                              "X    " );

    chars['s'] = new CharMask("     ",
                              "     ",
                              " xxxx",
                              "x    ",
                              " xxxX",
                              "    X",
                              "XXXX ");

    chars['t'] = new CharMask(" X   ",
                              " X   ",
                              "XXX  ",
                              " X   ",
                              " X   ",
                              " X  X",
                              "  XX " );

    chars['u'] = new CharMask("     ",
                              "     ",
                              "X   X",
                              "X   X",
                              "X   X",
                              "X  XX",
                              " XX X");

    // ü
    chars[0xF5] = new CharMask(" X X ",
                               "     ",
                               "X   X",
                               "X   X",
                               "X   X",
                               "X  XX",
                               " XX X");

    chars['v'] = new CharMask("     ",
                              "     ",
                              "X   X",
                              "X   X",
                              "X   X",
                              " X X ",
                              "  X  " );

    chars['w'] = new CharMask("     ",
                              "     ",
                              "X   X",
                              "X   X",
                              "X X X",
                              "X X X",
                              " X X ");

    chars['x'] = new CharMask("     ",
                              "     ",
                              "X   X",
                              " X X ",
                              "  X  ",
                              " X X ",
                              "X   X" );

    chars['y'] = new CharMask("     ",
                              "     ",
                              "X   X",
                              "X   X",
                              " XXX ",
                              "    X",
                              " XXX ");

    chars['z'] = new CharMask("     ",
                              "     ",
                              "XXXXX",
                              "   X ",
                              "  X  ",
                              " X   ",
                              "XXXXX" );

    chars['{'] = new CharMask("   X ",
                              "  X  ",
                              "  X  ",
                              " X   ",
                              "  X  ",
                              "  X  ",
                              "   X " );

    chars['|'] = new CharMask("  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ",
                              "  X  ");

    chars['}'] = new CharMask(" X   ",
                              "  X  ",
                              "  X  ",
                              "   X ",
                              "  X  ",
                              "  X  ",
                              " X   " );

    // 255 has all pixels active
    chars[255] = new CharMask("XXXXX",
                              "XXXXX",
                              "XXXXX",
                              "XXXXX",
                              "XXXXX",
                              "XXXXX",
                              "XXXXX",
                              "XXXXX" );

    // guarantee that all chars at least have a value
    for (int i = 0; i < 256; ++i)
        if (chars[i] == NULL)
            chars[i] = new CharMask(*chars['?']);
}
