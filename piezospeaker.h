#ifndef PIEZOSPEAKER_H
#define PIEZOSPEAKER_H

#include <QWidget>

#include "sensorinterface.h"

namespace Ui {
class PiezoSpeaker;
}

/**
 * @brief The PiezoSpeaker class
 * Just a visualization of the speaker frequency.
 */
class PiezoSpeaker : public SensorInterface
{
    Q_OBJECT

    Ui::PiezoSpeaker *uiSpeaker;

public:
    explicit PiezoSpeaker(QWidget *parent, const char *title, bool isV2);
    ~PiezoSpeaker();

public slots:
    // new frequency value from extern
    void updateUi(int);

    // new led value
    void updateLed(bool);

};

#endif // PIEZOSPEAKER_H
