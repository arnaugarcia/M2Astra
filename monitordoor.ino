#include <SPI.h>
#include <MCP2515_sw_can.h>

// Pin definitions specific to how the MCP2515 is wired up.
#ifdef MACCHINA_M2
#define CS_PIN  SPI0_CS3
#define INT_PIN SWC_INT
#else
#define CS_PIN  34
#define INT_PIN 38
#endif

// Create CAN object with pins as defined
SWcan SCAN(CS_PIN, INT_PIN);

void CANHandler() {
    SCAN.intHandler();
}

void setup() {
    SerialUSB.begin(115200);

    SerialUSB.println("Initializing ...");

    // Set up SPI Communication
    // dataMode can be SPI_MODE0 or SPI_MODE3 only for MCP2515
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.begin();

    // Initialize MCP2515 CAN controller at the specified speed and clock frequency
    // (Note:  This is the oscillator attached to the MCP2515, not the Arduino oscillator)
    // speed in KHz, clock in MHz
    SCAN.setupSW(33);

    attachInterrupt(INT_PIN, CANHandler, FALLING);
    SCAN.InitFilters(true);
    SCAN.mode(3);

    SerialUSB.println("Ready ...");
}

int count = 0;

// CAN message frame (actually just the parts that are exposed by the MCP2515 RX/TX buffers)
CAN_FRAME message;

void loop() {

    if (SCAN.GetRXFrame(message)) {
        // Print message
        if (message.id == 0x230) {
            count++;
            if (message.data.bytes[0] == 0x05 && message.data.bytes[5] == 0x09) {
                SerialUSB.println("Car locked");
            }
            if (message.data.bytes[0] == 0x00 && message.data.bytes[0] == 0x00) {
                SerialUSB.println("Car unlocked");
            }
        }
    }

}

void printFrame(CAN_FRAME message) {
    for(int i=0; i<message.length; i++) {
        SerialUSB.print(message.data.byte[i], HEX);
        SerialUSB.print(" ");
    }
}
