#include <ad7193.h>
#include <stdint.h>

AD7193 module;
void setup(){
    Serial.begin(9600);
    module.begin(2);

    ad7193_config_t config = module.getConfig();
    config.cr_gain = 0;
    config.cr_unipolar = true;
    AD7193::addChannelToConfig(&config, AD7193_CHAN2);
    module.setConfig(&config);

    delay(500);
}

void loop(){
    uint8_t channel;
    uint32_t data = module.getData(&channel);
    Serial.print("Channel ");
    Serial.println(channel, BIN);
    Serial.println(data, HEX);

    delay(1000);
}
