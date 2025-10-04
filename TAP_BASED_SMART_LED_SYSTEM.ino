#define BLYNK_TEMPLATE_ID "TMPL3EPcy8UyB"
#define BLYNK_TEMPLATE_NAME "Tap Based Smart LED"
#define BLYNK_AUTH_TOKEN "nzxu_3Z1ILc3JIDH6cTf8XVYHaSDxOgK"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define TOUCH_PIN     4
#define WHITE_LED     23
#define BLUE_LED      22
#define BUZZER        21

char ssid[] = "Student";
char pass[] = "Learn@123";

// States
int tapCount = 0;
unsigned long lastTapTime = 0;
unsigned long tapTimeout = 600;

bool whiteLEDState = false;
bool blueLEDState = false;
bool blinkMode = false;

unsigned long lastBlinkTime = 0;
bool blinkState = false;

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(WHITE_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);

  digitalWrite(WHITE_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(BUZZER, LOW);

  updateStatesToBlynk();
  Serial.println("Tap-Based Smart LED System Initialized");
}

BLYNK_CONNECTED() {
  updateStatesToBlynk();
}

void loop() {
  Blynk.run();

  // Handle blinking mode
  if (blinkMode && millis() - lastBlinkTime >= 500) {
    lastBlinkTime = millis();
    blinkState = !blinkState;
    digitalWrite(WHITE_LED, blinkState);
    digitalWrite(BLUE_LED, blinkState);
  }

  // Tap detection
  static bool lastTouchState = HIGH;
  bool currentTouchState = digitalRead(TOUCH_PIN);

  if (currentTouchState == LOW && lastTouchState == HIGH) {
    if (millis() - lastTapTime > 50) {
      tapCount++;
      lastTapTime = millis();
    }
  }
  lastTouchState = currentTouchState;

  if (tapCount > 0 && millis() - lastTapTime > tapTimeout) {
    handleTaps(tapCount);
    tapCount = 0;
  }
}

void handleTaps(int count) {
  Serial.print("Taps Detected: ");
  Serial.println(count);

  switch (count) {
    case 1:
      whiteLEDState = true;
      blinkMode = false;
      updateOutputs();
      beep();
      Serial.println("White LED ON");
      break;

    case 2:
      whiteLEDState = false;
      blinkMode = false;
      updateOutputs();
      beep();
      Serial.println("White LED OFF");
      break;

    case 3:
      blueLEDState = true;
      blinkMode = false;
      updateOutputs();
      beep();
      Serial.println("Blue LED ON");
      break;

    case 4:
      blueLEDState = false;
      blinkMode = false;
      updateOutputs();
      beep();
      Serial.println("Blue LED OFF");
      break;

    case 5:
      blinkMode = true;
      whiteLEDState = true;
      blueLEDState = true;
      Serial.println("Blinking Both LEDs");
      beep();
      break;

    case 6:
      blinkMode = false;
      whiteLEDState = false;
      blueLEDState = false;
      updateOutputs();
      beep();
      Serial.println("Blinking Mode OFF, Both LEDs OFF");
      break;

    default:
      Serial.println("No defined action for this tap count");
      break;
  }

  updateStatesToBlynk();
  printStatusToSerial();
}

void updateOutputs() {
  digitalWrite(WHITE_LED, whiteLEDState ? HIGH : LOW);
  digitalWrite(BLUE_LED, blueLEDState ? HIGH : LOW);
}

void beep() {
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
}

void updateStatesToBlynk() {
  Blynk.virtualWrite(V0, whiteLEDState ? (blinkMode ? "Blinking" : "ON") : "OFF");
  Blynk.virtualWrite(V1, blueLEDState ? (blinkMode ? "Blinking" : "ON") : "OFF");
}

void printStatusToSerial() {
  Serial.print("White LED: ");
  Serial.print(blinkMode ? "Blinking" : (whiteLEDState ? "ON" : "OFF"));
  Serial.print(" | Blue LED: ");
  Serial.println(blinkMode ? "Blinking" : (blueLEDState ? "ON" : "OFF"));
}
