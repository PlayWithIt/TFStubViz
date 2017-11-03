
#include <QPushButton>

#include "lcd.h"
#include "lcddisplayarea.h"

/**
 * Init the LCD-view for drawing, see also: http://qt-project.org/doc/qt-4.8/painting-basicdrawing.html
 */
LCD::LCD(QWidget *parent, const char *uid)
    : QWidget(parent)
{
    setupUi(this);

    renderArea = new LCDDisplayArea(this);
    gridLayout->addWidget(renderArea, 0, 0, 5, 6);

    QPushButton* buttons[4];
    for (int i = 0; i < 4; ++i)
    {
        QPushButton *btn = new QPushButton(this);
        btn->setText(QString("Button ") + QString::number(i));
        gridLayout->addWidget(btn, i, 7, 1, 1);
        buttons[i] = btn;
        renderArea->addButton(btn);
    }

    if (uid != NULL)
        groupBox->setTitle(QString("LCD 20x4 - ") + QString(uid));
    connectTooltipTo(groupBox, uid);

    // Connect the 4 buttons of the LCD.
    connect(buttons[0], &QPushButton::pressed,  this, [this]{ renderArea->buttonDown(0); });
    connect(buttons[0], &QPushButton::released, this, [this]{ renderArea->buttonUp(0); });
    connect(buttons[1], &QPushButton::pressed,  this, [this]{ renderArea->buttonDown(1); });
    connect(buttons[1], &QPushButton::released, this, [this]{ renderArea->buttonUp(1); });
    connect(buttons[2], &QPushButton::pressed,  this, [this]{ renderArea->buttonDown(2); });
    connect(buttons[2], &QPushButton::released, this, [this]{ renderArea->buttonUp(2); });
    connect(buttons[3], &QPushButton::pressed,  this, [this]{ renderArea->buttonDown(3); });
    connect(buttons[3], &QPushButton::released, this, [this]{ renderArea->buttonUp(3); });
}

LCD::~LCD() {
    delete renderArea;
}

/**
 * Sets the buttons labels from one string (splits the string into single labels).
 * @param label - label string with spaces as separator, '_' will be blank in the label
 */
void LCD::setLabels(const std::string &label) const
{
    renderArea->setLabels(label);
}
