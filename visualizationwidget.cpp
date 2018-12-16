
#include "QWidget"
#include "visualizationwidget.h"

VisualizationWidget::VisualizationWidget()
  : widget(NULL), position('?'), parent("?")
{ }

/**
 * Sets the parameter where this device is located in the brick stack.
 *
 * @param position - position a..d or 0..9
 * @param parent - parent UID
 */
void VisualizationWidget::setStackParameter(char position, const std::string &parent)
{
    this->position = position;
    this->parent = parent;

    if (widget) {
        if (parent.length() > 0 && position >= 'a')
            widget->setToolTip(QString("Device is on port '") + QString(position) + QString("' of ") + QString(parent.c_str()));
        else
            widget->setToolTip(QString("Device is at position '") + QString(position) + QString("' in the stack "));
    }
}
