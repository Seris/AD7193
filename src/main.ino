#include <ad7193.h>
#include <stdint.h>

AD7193 module;
void setup(){
    Serial.begin(9600);
    module.begin(2);
    delay(1000);
}

void loop(){
    Serial.println(module.getData());
    delay(1000);
}
