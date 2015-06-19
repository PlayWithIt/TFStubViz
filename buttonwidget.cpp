#include <QDebug>

#include "buttonwidget.h"

#include <utils/StringUtil.h>

/**
 * Default init.
 */
ButtonWidget::ButtonWidget(QWidget *parent)
    : QWidget(parent)
    , buttonState(0)
{
}


/**
 * Sets the buttons labels from one string (splits the string into single labels).
 * @param label - label string with spaces as separator, '_' will be blank in the label
 */
void ButtonWidget::setLabels(const std::string &label) const
{
    if (label.length() == 0)
        return;

    std::vector<std::string> items;
    utils::strings::split(label, ' ', items);

    unsigned i = 0;
    for (auto it : buttons)
    {
        if (i == items.size())
            return;
        it->setText(utils::strings::replaceAll(items[i], "\\n", "\n").c_str());
        ++i;
    }
}

void ButtonWidget::buttonDown(int no)
{
    buttonState |= (1 << no);
    // qDebug() << "Button " << no << " pressed, new state " << std::hex << buttonState;
}

void ButtonWidget::buttonUp(int no)
{
    buttonState ^= (1 << no);
    // qDebug() << "Button " << no << " released, new state " << std::hex << buttonState;
}


bool ButtonWidget::useAsInputSource(unsigned) const {
    return true;
}

int64_t ButtonWidget::getInputState(unsigned) const
{
    return buttonState;
}
