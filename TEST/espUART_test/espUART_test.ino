#define RXD2 22  
#define TXD2 35  

void setup() {
    Serial.begin(115200); 
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); 

    Serial.println("ESP32 UART2 Ready!");
}

void loop() {
    while (Serial2.available()) {  
        char receivedChar = Serial2.read();
        Serial.write(receivedChar);  
    }

    Serial2.println("Hello from ESP32!");
    delay(1000);
}
