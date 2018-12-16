
#include "oled.h"
#include "ui_oled.h"

/**
 * Size of the internal OLED border (just one Pixels)
 */
static const unsigned BORDER_SIZE = 1;

/**
 * Size for each pixel in the OLED screen in Pixels
 */
static const unsigned PIXEL_SIZE = 3;

using stubserver::DisplayState;

/**
 * Init UI, if 'small' is true, then 64x48 pixels are used, otherwise 128x64.
 */
OLED::OLED(QWidget *parent, const char *uid, bool small)
    : QWidget(parent)
    , whitePen(Qt::white, 1)
    , bluePen(Qt::darkBlue, 1)
    , grayPen(Qt::gray, 1)
    , ui(new Ui::OledForm)
    , lines(small ? 48 : 64)   // 8 lines per byte
    , cols(small ? 64 : 128)
    , isLcd(false)
    , mouseIsDown(false)
{
    ui->setupUi(this);

    char label[32];
    sprintf(label, "OLED %dx%d", cols, lines);
    ui->groupBox->setTitle(QString(label) + QString(" - ") + QString(uid));

    if (small) {
        QSize size(cols * PIXEL_SIZE + 2, lines * PIXEL_SIZE + 2);
        ui->screen->setMinimumSize(size);
        ui->screen->resize(size);
    }

    clear(false);
}

/**
 * Used to create the LCD 128x64.
 */
OLED::OLED(QWidget *parent, const char *uid)
    : QWidget(parent)
    , whitePen(Qt::black, 1)
    , bluePen(Qt::white, 1)
    , grayPen(Qt::gray, 1)
    , ui(new Ui::OledForm)
    , lines(64)   // 8 lines per byte
    , cols(128)
    , isLcd(true)
    , mouseIsDown(false)
{
    ui->setupUi(this);

    char label[32];
    sprintf(label, "LCD %dx%d", cols, lines);
    ui->groupBox->setTitle(QString(label) + QString(" - ") + QString(uid));

    clear(false);
}

OLED::~OLED() {
    delete ui;
}

void OLED::clear(bool inverted)
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, inverted ? whitePen.color() : bluePen.color());
    ui->screen->setAutoFillBackground(true);
    ui->screen->setPalette(Pal);
}


/**
 * Calculate the screen pixel coordinate out of the QWidget event position
 * @return true if the event was within the screen.
 */
bool OLED::getScreenPixel(QPoint &point, unsigned &col, unsigned &line) const
{
    unsigned border = ui->screen->x() + 7;
    unsigned mouse = point.rx();
    if (mouse < border)
        return false;
    col = (mouse - border) / PIXEL_SIZE;

    border = ui->screen->y() + 6;
    mouse = point.ry();
    if (mouse < border)
        return false;
    line = (mouse - border) / PIXEL_SIZE;

//  printf("getScreenPixel for %d %d, screen: %d %d => col %d, line %d\n",
//         point.rx(), point.ry(), ui->screen->x(), ui->screen->y(), col, line);

    return (col < cols && line < lines);
}

/**
 * Determine current mouse position (x,y pixel coordinate) and store
 * current time. The left button maybe used by Qt to move the window,
 * so we use the right button here !
 */
void OLED::mousePressEvent(QMouseEvent *event)
{
    mouseIsDown = false;
    if (isLcd && event->button() == Qt::RightButton) {
        unsigned col, line;
        QPoint mouseDown = event->pos();
        if (getScreenPixel(mouseDown, col, line)) {
            if (displayState)
                displayState->startTouch(col, line);
            mouseIsDown = true;
        }
    }
}

/**
 * Only in case of LCD:
 * if the release position is the same as press: trigger a touch event;
 * if the release position is another: trigger a gesture (switch left/right,top/down).
 */
void OLED::mouseReleaseEvent(QMouseEvent *event)
{
    // if mouse was pressed down outside: it is not true here
    if (mouseIsDown && event->button() == Qt::RightButton) {
        unsigned col, line;

        QPoint mouseDown = event->pos();
        if (getScreenPixel(mouseDown, col, line)) {
            if (displayState)
                displayState->endTouch(col, line);
            printf("mouse UP %u %d %c\n", displayState->getTouchX(), displayState->getTouchY(), displayState->getGesture());
        }
        mouseIsDown = false;
    }
}

/**
 * Redraw the screen with the current text content.
 */
void OLED::paintEvent(QPaintEvent * /* event */)
{
    if (displayState == NULL)
        return;

    bool inverted = displayState->isInverted();

    // set background color (blue as default, white if inverted)
    const QRect &rect = ui->screen->geometry();
    QPixmap pixmap(rect.width(), rect.height());
    pixmap.fill(inverted ? whitePen.color() : bluePen.color());
    QPainter painter(&pixmap);

    // Set pixel color
    QColor pixColor = inverted ? bluePen.color() : whitePen.color();

    for (unsigned line = 0; line < lines; ++line)
    {
        for (unsigned actCol = 0; actCol < cols; ++actCol)
        {
            bool isOn = displayState->isPixelOn(line, actCol);
            if (isOn) {
                // add 2 pixels to X and 1 for Y to have 1 pixel border on the screen
                QRect rect(1 + actCol * PIXEL_SIZE, 1 + line * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE);
                painter.fillRect(rect, pixColor);
            }
        }
    }
    ui->screen->setPixmap(pixmap);
}

/**
 * Consume update notification from stubserver.
 */
void OLED::notify(const stubserver::VisibleDeviceState &hint)
{
    if (hint.getChangeCode() == stubserver::VisibleDeviceState::DISCONNECT)
        displayState = NULL;
    else if (hint.getChangeCode() == stubserver::VisibleDeviceState::CONNECTED)
        displayState = dynamic_cast<const stubserver::DisplayState*>(&hint);
    else {
        update();
    }
}
