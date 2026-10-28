const int PHOTORESISTOR_PIN = 34;
const int HOLES_PER_REV = 6;
const unsigned long STOP_TIMEOUT_MS = 2000;  // no pulse for this long = stopped
const float MIN_SIGNAL_RANGE = 150;          // ADC counts; below this, no usable signal

const float SMOOTHING = 0.3;              // smooth ADC noise (1.0 = no smoothing)
const float ENVELOPE_DECAY = 0.001;       // ~1 s time constant at 1 ms per sample
const float HYSTERESIS_FRACTION = 0.1;    // of the min..max range, around the middle
const unsigned long REPORT_INTERVAL_MS = 200;

// Sensor signal
float filtered = 0;
float envelopeMin = 4095;
float envelopeMax = 0;

// Current hole being tracked
bool inHole = false;
float peak = 0;
unsigned long holeStartMs = 0;
unsigned long peakMs = 0;

// Rotation results
unsigned long lastHoleMs = 0;
unsigned long periodMs = 0;
const char *direction = "STOPPED";

unsigned long lastReportMs = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // 0-4095
  analogSetPinAttenuation(PHOTORESISTOR_PIN, ADC_11db);  // full 0-3.3V range
  filtered = analogRead(PHOTORESISTOR_PIN);
}

void readSensor() {
  filtered += (analogRead(PHOTORESISTOR_PIN) - filtered) * SMOOTHING;

  // Track min/max, slowly decaying toward the current value (~1 s time constant).
  envelopeMax = max(filtered, envelopeMax - (envelopeMax - filtered) * ENVELOPE_DECAY);
  envelopeMin = min(filtered, envelopeMin + (filtered - envelopeMin) * ENVELOPE_DECAY);
}

void startHole(unsigned long now) {
  inHole = true;
  periodMs = now - holeStartMs;
  holeStartMs = now;
  peak = filtered;
  peakMs = now;
}

// A teardrop's tail passes first when turning clockwise, so the brightest
// point comes late in the pulse. Counter-clockwise, it comes early.
void endHole(unsigned long now) {
  inHole = false;
  lastHoleMs = now;
  unsigned long timeBeforePeak = peakMs - holeStartMs;
  unsigned long timeAfterPeak = now - peakMs;
  direction = (timeBeforePeak > timeAfterPeak) ? "CW" : "CCW";
  Serial.printf("pulse dir=%s\n", direction);
}

void detectHoles(unsigned long now) {
  float range = envelopeMax - envelopeMin;
  if (range <= MIN_SIGNAL_RANGE) {
    return;
  }
  float middle = envelopeMin + range / 2;
  float hysteresis = range * HYSTERESIS_FRACTION;

  if (!inHole) {
    if (filtered > middle + hysteresis) {
      startHole(now);
    }
    return;
  }

  if (filtered > peak) {
    peak = filtered;
    peakMs = now;
  }
  if (filtered < middle - hysteresis) {
    endHole(now);
  }
}

void report(unsigned long now) {
  bool stopped = lastHoleMs == 0 || now - lastHoleMs > STOP_TIMEOUT_MS;
  float rpm = (stopped || periodMs == 0) ? 0 : 60000.0 / (periodMs * HOLES_PER_REV);
  Serial.printf("dir=%s rpm=%.1f holes=%d value=%d\n",
                stopped ? "STOPPED" : direction, rpm, HOLES_PER_REV, (int)filtered);
}

void loop() {
  unsigned long now = millis();
  readSensor();
  detectHoles(now);

  if (now - lastReportMs >= REPORT_INTERVAL_MS) {
    lastReportMs = now;
    report(now);
  }

  delay(1);
}
