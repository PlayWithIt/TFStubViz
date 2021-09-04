#ifndef SINGLEOUTDOORSENSOR_H
#define SINGLEOUTDOORSENSOR_H

#include <QCheckBox>
#include <QSlider>
#include <QLabel>

/**
 * Handles all elements for one groupbox of an outdoor sensor
 * with temperature, humidity and manual control checkbox.
 */
class SingleOutdoorSensor : public QWidget
{
    Q_OBJECT

    int    id;
    int    currentTemp;
    int    currentHumidity;
    bool   manualControl;

    QCheckBox *checkManual;
    QSlider   *sliderTemp;
    QSlider   *sliderHum;
    QLabel    *labelTemp;
    QLabel    *labelHum;
    QLabel    *labelId;

public:
    SingleOutdoorSensor(QWidget *parent, QCheckBox *checkBox, QSlider *sliderTemp, QSlider *sliderHum,
                        QLabel *labelTemp, QLabel *labelHum, QLabel *labelId);

    /**
     * @brief Initial connect the object and set ID and initial sensor values
     */
    void connectSensor(int id, int temp, int humidity) {
        this->id = id;
        this->currentTemp = temp;
        this->currentHumidity = humidity;
    }

    /**
     * Getter for currentTemp
     */
    int getCurrentTemp() const {
        return currentTemp;
    }

    /**
     * Getter for currentHumidity
     */
    int getCurrentHumidity() const {
        return currentHumidity;
    }

    /**
     * @brief return true if manual control via slider is active.
     */
    bool isManualControl() const {
        return manualControl;
    }

signals:
    void newValues(int temp, int hum);

public slots:
    // new sensor values for channel (updates id + temp + hum)
    void updateAll();
    void updateSensors(int temp, int hum);

    // slider moves for temperature and humidity
    void tempSliderMoved(int value);
    void humSliderMoved(int value);

    // slider changed
    void tempActionTriggered(int action);
    void humActionTriggered(int action);

    // slider touched, activate manual control
    void activateManualControl();

    // change state of manual control for the sliders
    void toggleManualControl(int v);
};

#endif // SINGLEOUTDOORSENSOR_H
