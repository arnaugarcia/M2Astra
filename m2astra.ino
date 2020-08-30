#include <Arduino.h>
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
    delay(5000);
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
    //speed in KHz, clock in MHz
    SCAN.setupSW(33);

    attachInterrupt(INT_PIN, CANHandler, FALLING);
    SCAN.InitFilters(true);
    SCAN.mode(3); //3 = Normal, 2 = HV Wake up, 1 = High Speed, 0 = Sleep

    SerialUSB.println("Ready ...");
}

byte i=0;

// CAN message frame
CAN_FRAME message;

void loop() {
    unlockdoor();
    delay(2000);
    SCAN.mode(2); //use HV Wakeup sending
    lockdoor();
    delay(5); //wait a bit to make sure it was sent before ending HV Wake up
    SCAN.mode(3); //go back to normal mode
    delay(2000);
}

void unlockdoor() {
    SerialUSB.println("Unlocking ...");
    message.id = 0x230;
    message.rtr = 0;
    message.extended = 0;
    message.length = 7;
    message.data.byte[0] = 0x00;
    message.data.byte[1] = 0x00;
    message.data.byte[2] = 0x00;
    message.data.byte[3] = 0x00;
    message.data.byte[4] = 0x00;
    message.data.byte[5] = 0x00;
    message.data.byte[6] = 0x00;
    SCAN.EnqueueTX(message);
}

void lockdoor() {
    SerialUSB.println("locking ...");
    message.id = 0x230;
    message.rtr = 0;
    message.extended = 0;
    message.length = 7;
    message.data.byte[0] = 0x05;
    message.data.byte[1] = 0x00;
    message.data.byte[2] = 0x00;
    message.data.byte[3] = 0x00;
    message.data.byte[4] = 0x00;
    message.data.byte[5] = 0x09;
    message.data.byte[6] = 0x00;
    SCAN.EnqueueTX(message);
}