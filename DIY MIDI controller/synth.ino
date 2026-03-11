#include <Wire.h>
#include "Adafruit_Trellis.h"

// ---- Trellis config ----
#define NUMTRELLIS 4
#define MOMENTARY 0
#define LATCHING  1
#define MODE      MOMENTARY

Adafruit_Trellis matrix[NUMTRELLIS] = {
  Adafruit_Trellis(), Adafruit_Trellis(),
  Adafruit_Trellis(), Adafruit_Trellis()
};

Adafruit_TrellisSet trellis = Adafruit_TrellisSet(
  &matrix[0], &matrix[1], &matrix[2], &matrix[3]
);

#define numKeys (NUMTRELLIS * 16)

// ---- MIDI settings ----
const byte MIDI_CH   = 0;    // 0..15 (0 = Channel 1)
const byte NOTE_BASE = 36;   // 36 = C2
const byte NOTE_VEL  = 100;  // 0..127

// ---- Pot settings ----
const byte POT_PIN = A0;
const byte POT_CC  = 1;      // CC1 = Mod Wheel (try 74 for filter cutoff)
int lastCC = -1;
unsigned long lastPotSend = 0;

// ---- Raw MIDI over UART helpers ----
void noteOn(byte note, byte vel) {
  Serial.write((byte)(0x90 | MIDI_CH));
  Serial.write(note);
  Serial.write(vel);
}

void noteOff(byte note) {
  Serial.write((byte)(0x80 | MIDI_CH));
  Serial.write(note);
  Serial.write((byte)0);
}

void controlChange(byte cc, byte value) {
  Serial.write((byte)(0xB0 | MIDI_CH)); // CC message
  Serial.write(cc);
  Serial.write(value);
}

void setup() {
  Serial.begin(31250);
  trellis.begin(0x70, 0x71, 0x72, 0x73);

  // Optional startup animation
  for (uint8_t i = 0; i < numKeys; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();
    delay(30);
  }
  for (uint8_t i = 0; i < numKeys; i++) {
    trellis.clrLED(i);
    trellis.writeDisplay();
    delay(30);
  }
}

void readPotAndSendCC() {
  // Read raw 0..1023
  int raw = analogRead(POT_PIN);

  // Map to MIDI 0..127
  int ccVal = map(raw, 0, 1023, 0, 127);

  // Simple anti-jitter: only send if changed by >= 1 step,
  // and don't send more than ~100 times/sec
  unsigned long now = millis();
  if (ccVal != lastCC && (now - lastPotSend) >= 10) {
    controlChange(POT_CC, (byte)ccVal);
    lastCC = ccVal;
    lastPotSend = now;
  }
}

void loop() {
  delay(30); // required by Trellis

  // Always read the pot (not only when buttons change)
  readPotAndSendCC();

  if (trellis.readSwitches()) {
    for (uint8_t i = 0; i < numKeys; i++) {
      byte note = NOTE_BASE + i;

      if (MODE == MOMENTARY) {
        if (trellis.justPressed(i)) {
          trellis.setLED(i);
          noteOn(note, NOTE_VEL);
        }
        if (trellis.justReleased(i)) {
          trellis.clrLED(i);
          noteOff(note);
        }
      }

      if (MODE == LATCHING) {
        if (trellis.justPressed(i)) {
          if (trellis.isLED(i)) {
            trellis.clrLED(i);
            noteOff(note);
          } else {
            trellis.setLED(i);
            noteOn(note, NOTE_VEL);
          }
        }
      }
    }
    trellis.writeDisplay();
  }
}
