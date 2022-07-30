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
    std::string parent;

protected:
    VisualizationWidget();

    /**
     * The widget which is registered here is used to create a tooltip for
     * when "setStackParameter()" is called. This cannot be used at construction
     * time as the whole widget hierarchy might is created after this class
     * gets constructed.
     */
    void connectTooltipTo(QWidget *w) {
        widget = w;
    }

public:
    /**
     * Sets the parameter where this device is located in the brick stack.
     *
     * @param position - position A..I or 0..9
     * @param parent - parent UID
     */
    virtual void setStackParameter(char position, const std::string &parent);
};

#endif // VISUALIZATIONWIDGET_H

