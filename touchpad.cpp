
#include <QDebug>
#include <iomanip>

#include "touchpad.h"


TouchPad::TouchPad(QWidget *parent, const char *uid)
    : ButtonWidget(parent)
{
    setupUi(this);

    addButton(button0);
    addButton(button1);
    addButton(button2);
    addButton(button3);
    addButton(button4);
    addButton(button5);
    addButton(button6);
    addButton(button7);
    addButton(button8);
    addButton(button9);
    addButton(button10);
    addButton(button11);

    connect(button0, &QPushButton::pressed,  this, [this]{ buttonDown(0); });
    connect(button0, &QPushButton::released, this, [this]{ buttonUp(0); });
    connect(button1, &QPushButton::pressed,  this, [this]{ buttonDown(1); });
    connect(button1, &QPushButton::released, this, [this]{ buttonUp(1); });
    connect(button2, &QPushButton::pressed,  this, [this]{ buttonDown(2); });
    connect(button2, &QPushButton::released, this, [this]{ buttonUp(2); });
    connect(button3, &QPushButton::pressed,  this, [this]{ buttonDown(3); });
    connect(button3, &QPushButton::released, this, [this]{ buttonUp(3); });
    connect(button4, &QPushButton::pressed,  this, [this]{ buttonDown(4); });
    connect(button4, &QPushButton::released, this, [this]{ buttonUp(4); });
    connect(button5, &QPushButton::pressed,  this, [this]{ buttonDown(5); });
    connect(button5, &QPushButton::released, this, [this]{ buttonUp(5); });
    connect(button6, &QPushButton::pressed,  this, [this]{ buttonDown(6); });
    connect(button6, &QPushButton::released, this, [this]{ buttonUp(6); });
    connect(button7, &QPushButton::pressed,  this, [this]{ buttonDown(7); });
    connect(button7, &QPushButton::released, this, [this]{ buttonUp(7); });
    connect(button8, &QPushButton::pressed,  this, [this]{ buttonDown(8); });
    connect(button8, &QPushButton::released, this, [this]{ buttonUp(8); });
    connect(button9, &QPushButton::pressed,  this, [this]{ buttonDown(9); });
    connect(button9, &QPushButton::released, this, [this]{ buttonUp(9); });
    connect(button10, &QPushButton::pressed,  this, [this]{ buttonDown(10); });
    connect(button10, &QPushButton::released, this, [this]{ buttonUp(10); });
    connect(button11, &QPushButton::pressed,  this, [this]{ buttonDown(11); });
    connect(button11, &QPushButton::released, this, [this]{ buttonUp(11); });

    if (uid != NULL)
        groupBox->setTitle(QString("MULTI_TOUCH - ") + QString(uid));
    connectTooltipTo(groupBox, uid);
}
