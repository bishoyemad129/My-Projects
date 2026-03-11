#include <Servo.h>
#include <Wire.h>

Servo myservo;

// IR sensor pins
#define ir_enter 2
#define ir_back  4
#define ir_car1 11
#define ir_car2 12
#define ir_car3 13

// Slot LED pins
const int greenLed1 = A0, redLed1 = A1;
const int greenLed2 = A2, redLed2 = A3;
const int greenLed3 = 8,  redLed3 = 9;

// Gate status LED pins
const int greenGateLED  = 5;
const int yellowGateLED = 6;
const int redGateLED    = 7;

// Slot occupancy state
int S1 = 0, S2 = 0, S3 = 0;

// Entry/Exit flags
int flagEntry = 0;
int flagExit = 0;

// Entry time tracking
unsigned long entryTime1 = 0;
unsigned long entryTime2 = 0;
unsigned long entryTime3 = 0;

// For tracking state changes
int prevS1 = -1, prevS2 = -1, prevS3 = -1;
int prevFreeSlots = 3;
bool pendingExitDetected = false;

void setup() {
  Serial.begin(9600);

  // IR sensor pin modes
  pinMode(ir_enter, INPUT);
  pinMode(ir_back, INPUT);
  pinMode(ir_car1, INPUT);
  pinMode(ir_car2, INPUT);
  pinMode(ir_car3, INPUT);

  // Slot LED pin modes
  pinMode(greenLed1, OUTPUT); pinMode(redLed1, OUTPUT);
  pinMode(greenLed2, OUTPUT); pinMode(redLed2, OUTPUT);
  pinMode(greenLed3, OUTPUT); pinMode(redLed3, OUTPUT);

  // Gate LED pin modes
  pinMode(greenGateLED, OUTPUT);
  pinMode(yellowGateLED, OUTPUT);
  pinMode(redGateLED, OUTPUT);

  // Initialize slot LEDs (all empty)
  digitalWrite(greenLed1, HIGH); digitalWrite(redLed1, LOW);
  digitalWrite(greenLed2, HIGH); digitalWrite(redLed2, LOW);
  digitalWrite(greenLed3, HIGH); digitalWrite(redLed3, LOW);

  // Initialize servo
  myservo.attach(3);
  myservo.write(90); // Gate closed

  Serial.println("Parking System Initialized");
}

void loop() {
  ReadSensors();
  ProcessEntryExit();
  UpdateLEDs();
  CheckSlotTimers();
  UpdateSerialMonitor();
  UpdateGateLEDs();
  delay(10);
}

void ReadSensors() {
  S1 = digitalRead(ir_car1) == LOW ? 1 : 0;
  S2 = digitalRead(ir_car2) == LOW ? 1 : 0;
  S3 = digitalRead(ir_car3) == LOW ? 1 : 0;
}

void ProcessEntryExit() {
  int freeSlots = GetFreeSlots();

  // Detect a new free slot without exit sensor triggered
  if (prevFreeSlots == 0 && freeSlots > 0 && !flagExit) {
    pendingExitDetected = true;
  }

  // Entry condition
  if (digitalRead(ir_enter) == LOW && flagEntry == 0) {
    flagEntry = 1;
    if (freeSlots > 0) {
      myservo.write(180);  // Open gate
    } else {
      Serial.println("Sorry, Parking Full!");
      delay(1500);
    }
  }

  // Exit condition
  if (digitalRead(ir_back) == LOW && flagExit == 0) {
    flagExit = 1;
    myservo.write(180);  // Open gate
    pendingExitDetected = false; // Exit confirmed
  }

  // Close gate after action
  if (flagEntry == 1 && flagExit == 1) {
    delay(1000);
    myservo.write(90);  // Close gate
    flagEntry = 0;
    flagExit = 0;
  }

  prevFreeSlots = freeSlots;
}

void UpdateLEDs() {
  digitalWrite(redLed1, S1 ? HIGH : LOW);
  digitalWrite(greenLed1, S1 ? LOW : HIGH);

  digitalWrite(redLed2, S2 ? HIGH : LOW);
  digitalWrite(greenLed2, S2 ? LOW : HIGH);

  digitalWrite(redLed3, S3 ? HIGH : LOW);
  digitalWrite(greenLed3, S3 ? LOW : HIGH);
}

void CheckSlotTimers() {
  CheckSlotTimer(S3, entryTime1, 1);
  CheckSlotTimer(S2, entryTime2, 2);
  CheckSlotTimer(S1, entryTime3, 3);
}

void CheckSlotTimer(int slotState, unsi/gned long &entryTime, int slotNumber) {
  if (slotState == 1 && entryTime == 0) {
    entryTime = millis(); // Car entered
  }
  if (slotState == 0 && entryTime != 0) {
    unsigned long duration = (millis() - entryTime) / 60000; // in minutes
    int fee = max(1, duration); // At least $1
    Serial.print("Slot ");
    Serial.print(slotNumber);
    Serial.print(" car exited. Duration: ");
    Serial.print(duration);
    Serial.print(" minute(s). Fee: $");
    Serial.println(fee);
    entryTime = 0;
  }
}

int GetFreeSlots() {
  return 3 - (S1 + S2 + S3);
}

void UpdateSerialMonitor() {
  int freeSlots = GetFreeSlots();

  if (S1 != prevS1 || S2 != prevS2 || S3 != prevS3 || freeSlots != prevFreeSlots) {
    Serial.println("========== Parking Slot Status ==========");
    Serial.print("Available Slots: ");
    Serial.print(freeSlots);
    Serial.println("/3");

    Serial.print("S1: "); Serial.println(S3 ? "Occupied" : "Empty");
    Serial.print("S2: "); Serial.println(S2 ? "Occupied" : "Empty");
    Serial.print("S3: "); Serial.println(S1 ? "Occupied" : "Empty");
    Serial.println("=========================================\n");

    prevS1 = S1;
    prevS2 = S2;
    prevS3 = S3;
    prevFreeSlots = freeSlots;
  }
}

void UpdateGateLEDs() {
  int freeSlots = GetFreeSlots();

  if (freeSlots > 0 && pendingExitDetected) {
    // Case: car left slot but didn't pass exit sensor
    digitalWrite(greenGateLED, LOW);
    digitalWrite(yellowGateLED, HIGH);
    digitalWrite(redGateLED, HIGH);
  } else if (freeSlots == 0) {
    // All slots full
    digitalWrite(greenGateLED, LOW);
    digitalWrite(yellowGateLED, LOW);
    digitalWrite(redGateLED, HIGH);
  } else {
    // Slots available and exit confirmed
    digitalWrite(greenGateLED, HIGH);
    digitalWrite(yellowGateLED, LOW);
    digitalWrite(redGateLED, LOW);
  }
}
