
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

/**
 * Init UI, if 'small' is true, then 64x48 pixels are used, otherwise 128x64.
 */
OLED::OLED(QWidget *parent, const char *uid, bool small)
    : QWidget(parent)
    , whitePen(Qt::white, 1)
    , grayPen(Qt::gray, 1)
    , ui(new Ui::OledForm)
    , lines(small ? 48 : 64)   // 8 lines per byte
    , cols(small ? 64 : 128)
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

    clear();
}


void OLED::clear()
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::darkBlue);
    ui->screen->setAutoFillBackground(true);
    ui->screen->setPalette(Pal);
}

/**
 * Redraw the screen with the current text content.
 */
void OLED::paintEvent(QPaintEvent * /* event */)
{
    utils::MutexLock lock(myMutex);
    if (oledState == NULL)
        return;

    QPixmap pixmap(cols*PIXEL_SIZE, lines*PIXEL_SIZE);
    pixmap.fill(Qt::darkBlue);

    // Max number of points
    QPainter painter(&pixmap);
    painter.setPen(whitePen);

    for (unsigned line = 0; line < lines; ++line)
    {
        for (unsigned actCol = 0; actCol < cols; ++actCol)
        {
            bool isOn = oledState->isPixelOn(line, actCol);
            if (!isOn)
                continue;

            // add 2 pixels to X and 1 for Y to have 1 pixel border on the screen
            QRect rect(1 + actCol * PIXEL_SIZE, 1 + line * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE);
            painter.fillRect(rect, Qt::GlobalColor::white);
        }
    }
    ui->screen->setPixmap(pixmap);
}

/**
 * Consume update notification from stubserver.
 */
void OLED::notify(const stubserver::VisibleDeviceState &hint)
{
    utils::MutexLock lock(myMutex);
    if (hint.getChangeCode() == stubserver::VisibleDeviceState::DISCONNECT)
        oledState = NULL;
    else {
        oledState = dynamic_cast<const stubserver::OledState*>(&hint);
        update();
    }
}
