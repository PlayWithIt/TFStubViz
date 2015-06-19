#ifndef LCD_H
#define LCD_H

#include "ui_lcd.h"

class LCDDisplayArea;

/**
 * @brief The LCD class with dotted charset
 *
 */
class LCD : public QWidget, private Ui::LcdForm
{
public:

    /**
     * Max. number of lines (rows) in the LCD
     */
    static const unsigned DISPLAY_LINES  = 4;

    /**
     * Max. number of chars in one line of the LCD
     */
    static const unsigned DISPLAY_COLS  = 20;

    /**
     * @brief LCD default init with a parent
     *
     * Default init with blue background, empty sreen.
     *
     * @param parent the Qt parent object
     */
    explicit LCD(QWidget *parent, const char *uid);

    LCDDisplayArea* getLCDDisplayArea() {
        return renderArea;
    }

    /**
     * Sets the buttons labels from one string (splits the string into single labels).
     * @param label - label string with spaces as separator, '_' will be blank in the label
     */
    void setLabels(const std::string &label) const;

private:
    Q_OBJECT

    LCDDisplayArea *renderArea;
};

#endif // LCD_H
