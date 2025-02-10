#include <SPI.h>
#include <mcp2515.h>

#define PT_CAN_CS 9
#define PT_CAN_INT 3
#define K_CAN_CS 8
#define K_CAN_INT 2

MCP2515 PT_CAN(PT_CAN_CS);
MCP2515 K_CAN(K_CAN_CS);

int speed = 0;          // Speed in km/h
int rpm = 0;            // RPM
int gasPedalPos = 0;    // Gas pedal position (0-100%)
int coolantTemp = 0;    // Coolant temperature in °C

// K-CAN Variables
float outsideTemp = 0;    // Outside temperature in °C
float insideTemp = 0;     // Inside temperature in °C
float fuelRange = 0;      // Fuel range in km
float avgConsumption = 0; // Average fuel consumption in L/100km
float batteryVoltage = 0; // Battery voltage in V
bool ignitionStatus = false;
bool prevIgnitionStatus = false;

unsigned long lastUpdateTime = 0;  // Last time data was sent to serial
const unsigned long updateInterval = 300;  //interval for data updates

void setup() {
    Serial.begin(115200);   // Start serial communication
    while (!Serial);

    // Initialize PT-CAN
    PT_CAN.reset();
    PT_CAN.setBitrate(CAN_500KBPS, MCP_8MHZ);
    PT_CAN.setNormalMode();
    Serial.println("PT-CAN Initialized at 500kbps.");

    // Initialize K-CAN
    K_CAN.reset();
    K_CAN.setBitrate(CAN_100KBPS, MCP_8MHZ);
    K_CAN.setNormalMode();
    Serial.println("K-CAN Initialized at 100kbps.");
}

void loop() {
    struct can_frame canMsg;
    if (PT_CAN.readMessage(&canMsg) == MCP2515::ERROR_OK) {
        processPT_CAN(canMsg);
    }

    if (K_CAN.readMessage(&canMsg) == MCP2515::ERROR_OK) {
        processK_CAN(canMsg);
    }

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;
        logData();
    }
}

void processPT_CAN(struct can_frame canMsg) {
    switch (canMsg.can_id) {
        case 0x1B4:  // Speed
            if (canMsg.can_dlc >= 2) {
                int speed_raw = ((canMsg.data[1] << 8) | canMsg.data[0]);
                speed = (((speed_raw - 0x0C00) / 16.0) * 1.61) + 1545.6;  // Convert to km/h
            }
            break;

        case 0x0AA:  // RPM and gas pedal position
            if (canMsg.can_dlc >= 6) {
                rpm = ((canMsg.data[5] << 8) | canMsg.data[4]) / 4.0;
            }
            if (canMsg.can_dlc >= 4) {
                uint16_t pedalPosRaw = (canMsg.data[3] << 8) | canMsg.data[2];
                const uint16_t deadZone = 800;
                const uint16_t minValue = 255 + deadZone;
                const uint16_t maxValue = 65064 - deadZone;
                if (pedalPosRaw < minValue) {
                    gasPedalPos = 0.0;
                } else if (pedalPosRaw > maxValue) {
                    gasPedalPos = 100.0;
                } else {
                    gasPedalPos = ((pedalPosRaw - minValue) * 100.0) / (maxValue - minValue);
                }
            }
            break;

        case 0x1D0:  // Coolant Temperature
            if (canMsg.can_dlc >= 1) {
                coolantTemp = canMsg.data[0] - 48;
            }
            break;

        default:
            break;
    }
}

void processK_CAN(struct can_frame canMsg) {
    switch (canMsg.can_id) {
        case 0x2CA:  // Outside temperature
            if (canMsg.can_dlc >= 1) {
                outsideTemp = (canMsg.data[0] - 80) / 2.0;
            }
            break;

        case 0x32E:  // Inside temperature
            if (canMsg.can_dlc >= 4) {
                int tempRaw = canMsg.data[2];
                insideTemp = (tempRaw / 10.0) + 6;
            }
            break;

        case 0x366:  // Fuel range
            if (canMsg.can_dlc >= 3) {
                int fuelRangeRaw = (canMsg.data[2] << 8) | canMsg.data[1];
                fuelRange = (fuelRangeRaw / 16.0);
            }
            break;

        case 0x362:  // Average consumption
            if (canMsg.can_dlc >= 3) {
                int avgConsumptionRaw = (canMsg.data[2] << 4) | (canMsg.data[1] >> 4);
                avgConsumption = avgConsumptionRaw / 10.0;
                if(avgConsumption > 100){
                    avgConsumption = 0;
                }
            }
            break;

        case 0x130:  // Ignition status
            if (canMsg.can_dlc >= 1) {
                ignitionStatus = canMsg.data[0] & 0x01;
                if (ignitionStatus != prevIgnitionStatus) {
                    prevIgnitionStatus = ignitionStatus;
                }
            }
            break;

        case 0x3B4:  // Battery voltage
            if (canMsg.can_dlc >= 2) {
                int voltageRaw = (((canMsg.data[1] - 240) * 256) + canMsg.data[0]);
                batteryVoltage = voltageRaw / 68.0;
            }
            break;


        default: 
            break;
    }
}

void logData() {
    //PT-CAN
    Serial.print(" SP:"); Serial.print(speed);
    Serial.print(" RPM:"); Serial.print(rpm);
    Serial.print(" CT:"); Serial.print(coolantTemp);
    Serial.print(" GP:"); Serial.println(gasPedalPos);

    //K-CAN
    Serial.print(" TO:"); Serial.print(outsideTemp);
    Serial.print(" TI:"); Serial.print(insideTemp);
    Serial.print(" FR:"); Serial.print(fuelRange);
    Serial.print(" AC:"); Serial.print(avgConsumption);
    Serial.print(" BV:"); Serial.print(batteryVoltage);
    Serial.print(" IS:"); Serial.println(ignitionStatus);
}

