
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
    , bluePen(Qt::darkBlue, 1)
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

    clear(false);
}

OLED::~OLED() {
    delete ui;
}

void OLED::clear(bool inverted)
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, inverted ? Qt::white : Qt::darkBlue);
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

    bool inverted = oledState->isInverted();

    // set background color (blue as default, white if inverted)
    const QRect &rect = ui->screen->geometry();
    QPixmap pixmap(rect.width(), rect.height());
    pixmap.fill(inverted ? Qt::white : Qt::darkBlue);
    QPainter painter(&pixmap);

    // Set pixel color
    Qt::GlobalColor pixColor = inverted ? Qt::GlobalColor::darkBlue : Qt::GlobalColor::white;

    for (unsigned line = 0; line < lines; ++line)
    {
        for (unsigned actCol = 0; actCol < cols; ++actCol)
        {
            bool isOn = oledState->isPixelOn(line, actCol);
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
        oledState = NULL;
    else if (hint.getChangeCode() == stubserver::VisibleDeviceState::CONNECTED)
        oledState = dynamic_cast<const stubserver::OledState*>(&hint);
    else {
        update();
    }
}
