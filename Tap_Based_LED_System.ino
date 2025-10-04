#define BLYNK_TEMPLATE_ID "TMPL3EPcy8UyB"
#define BLYNK_TEMPLATE_NAME "Tap Based Smart LED"
#define BLYNK_AUTH_TOKEN "nzxu_3Z1ILc3JIDH6cTf8XVYHaSDxOgK"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Pin Definitions
#define TOUCH_PIN     4
#define WHITE_LED     23
#define BLUE_LED      22
#define BUZZER        21

// WiFi Credentials
char ssid[] = "Student";
char pass[] = "Learn@123";

// Tap Handling Variables
int tapCount = 0;
unsigned long lastTapTime = 0;
unsigned long tapTimeout = 600;
bool whiteLEDState = false;
bool blueLEDState = false;

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(WHITE_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);

  whiteLEDState = false;
  blueLEDState = false;
  digitalWrite(WHITE_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(BUZZER, LOW);

  updateStatesToBlynk();  // Send initial states to Blynk

  Serial.println("Tap-Based Smart LED System Initialized");
}

// Blynk connection handler
BLYNK_CONNECTED() {
  updateStatesToBlynk();  // Sync current states on connection
}

void loop() {
  Blynk.run();

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
      digitalWrite(WHITE_LED, HIGH);
      beep();
      Serial.println("White LED ON");
      break;
    case 2:
      whiteLEDState = false;
      digitalWrite(WHITE_LED, LOW);
      beep();
      Serial.println("White LED OFF");
      break;
    case 3:
      blueLEDState = true;
      digitalWrite(BLUE_LED, HIGH);
      beep();
      Serial.println("Blue LED ON");
      break;
    case 4:
      blueLEDState = false;
      digitalWrite(BLUE_LED, LOW);
      beep();
      Serial.println("Blue LED OFF");
      break;
    default:
      Serial.println("No defined action for this tap count");
      break;
  }

  updateStatesToBlynk();
  printStatusToSerial();
}

void beep() {
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
}

void updateStatesToBlynk() {
  Blynk.virtualWrite(V0, whiteLEDState ? "ON" : "OFF");
  Blynk.virtualWrite(V1, blueLEDState ? "ON" : "OFF");
}

void printStatusToSerial() {
  Serial.print("White LED: ");
  Serial.print(whiteLEDState ? "ON" : "OFF");
  Serial.print(" | Blue LED: ");
  Serial.println(blueLEDState ? "ON" : "OFF");
}
