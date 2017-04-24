#include <ad7193.h>
#include <stdint.h>

AD7193 module;
ad7193_config_t config;

void setup(){
    Serial.begin(9600);
    module.begin(2);

    config = module.getConfig();
    AD7193::setChannelInConfig(&config, AD7193_CHAN0);
    config.cr_gain = 0;
    config.cr_chop = true;
    config.cr_unipolar = true;
    config.cr_pseudo = true;
    module.setConfig(&config);

    if(module.fullScaleCalibration() && module.calibrateChannel(AD7193_CHAN0)){
        Serial.println("Calibration successfull !");
    } else {
        Serial.println("Calibration failure !");
    }
}

void loop(){
    ad7193_chan_t channel;
    double data = module.getDataAsMilliVolts(NULL);
    if(data != AD7193_NODATA){
        Serial.println(data, HEX);
    } else {
        Serial.println("No data available");
    }
    delay(500);
}
