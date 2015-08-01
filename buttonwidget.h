#ifndef BUTTONWIDGET_H
#define BUTTONWIDGET_H

#include <QPushButton>
#include <list>

#include "visualizationwidget.h"

/**
 * The ButtonWidget is used to handle button input from an array of buttons
 * and holds a bitwise state per button. This value can be redirected to
 * the stubserver.
 * <P>
 * The button itself is not created here, but in a parent class. The parent
 * must connect the public slots of this class.
 */
class ButtonWidget : public QWidget, public VisualizationWidget
{
    Q_OBJECT

    uint64_t buttonState;
    std::list<QPushButton*> buttons;

public:
    explicit ButtonWidget(QWidget *parent);

    /**
     * Register a button: this can be used to be able to set the button labels if necessary.
     * The method must be called before 'setLabels'.
     */
    void addButton(QPushButton* btn) {
        buttons.push_back(btn);
    }

    /**
     * Sets the buttons labels from one string (splits the string into single labels).
     * @param label - label string with spaces as separator, '_' will be blank in the label
     */
    void setLabels(const std::string &label) const;

    //----- derived from VisualizationClient
    virtual bool useAsInputSource(unsigned sensorNo) const override;
    virtual int64_t getInputState(unsigned sensorNo) const override;

public slots:
    void buttonDown(int);
    void buttonUp(int);
};

#endif // BUTTONWIDGET_H
