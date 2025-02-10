#include <SPI.h>
#include <mcp2515.h>

void logData() {
    Serial.print("SP:"); Serial.print(random(300));
    Serial.print(" RPM:"); Serial.print(random(800, 5500));
    Serial.print(" CT:"); Serial.print(random(110));
    Serial.print(" GP:"); Serial.print(random(100));
    Serial.println();
    Serial.print("TO:"); Serial.print(random(15, 30));
    Serial.print(" TI:"); Serial.print(random(15, 30));
    Serial.print(" FR:"); Serial.print(random(400));
    Serial.print(" AC:"); Serial.print(random(5, 10));
    Serial.print(" IS:"); Serial.print(1);

    Serial.println();

}

