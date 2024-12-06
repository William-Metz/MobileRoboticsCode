#include <QMC5883LCompass.h>

class Compass {
    QMC5883LCompass compass;

    public:
    void setup() {
        compass.init();
        compass.setCalibrationOffsets(-324.00, 380.00, -848.00);
        compass.setCalibrationScales(1.60, 1.36, 0.61);
    }
    void loopStep(){
        compass.read();

    }
    float getTheta(){
        return compass.getAzimuth();

    }
};



