#ifndef VISUALIZATIONWIDGET_H
#define VISUALIZATIONWIDGET_H

#include <stubserver/VisualizationClient.h>

class QWidget;

/**
 * Base class for all Qt widget that are used as VisualizationClient: it adds
 * some methods in order to store the position and builds a tooltip.
 */
class VisualizationWidget : public stubserver::VisualizationClient
{
    QWidget *widget;

    char position;
    std::string uid;
    std::string parent;

protected:
    VisualizationWidget();

    void connectTooltipTo(QWidget *w, const char *uid) {
        widget = w;
        if (uid)
            this->uid = uid;
    }

public:
    /**
     * Sets the parameter where this device is located in the brick stack.
     *
     * @param position - position a..d or 0..9
     * @param parent - parent UID
     */
    virtual void setStackParameter(char position, const std::string &parent);
};

#endif // VISUALIZATIONWIDGET_H

