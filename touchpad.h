#ifndef TOUCHPAD_H
#define TOUCHPAD_H

#include "ui_touchpad.h"
#include "buttonwidget.h"

/**
 * @brief The TouchPad class
 *
 * This class simulates a touchpad with 12 buttons.
 */
class TouchPad : public ButtonWidget, private Ui::TouchPadForm
{
    Q_OBJECT

public:
    explicit TouchPad(QWidget *parent, const char *uid);
};

#endif // TOUCHPAD_H
