#include "mp3tf16p.h"

MP3Player mp3(10, 11);

#define TRIG_L 2
#define ECHO_L 3
#define TRIG_F 4
#define ECHO_F 5
#define TRIG_R 6
#define ECHO_R 7
#define MOTOR_PIN 12
#define DANGER_DIST 30
#define CRITICAL_DIST 15

long readSensor(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000);
  if (duration == 0) return 400;
  return duration * 0.034 / 2;
}

void vibrate(int pulses, int dur) {
  for (int i = 0; i < pulses; i++) {
    digitalWrite(MOTOR_PIN, HIGH);
    delay(dur);
    digitalWrite(MOTOR_PIN, LOW);
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);
  mp3.initialize();
  delay(500);

  pinMode(TRIG_L, OUTPUT); pinMode(ECHO_L, INPUT);
  pinMode(TRIG_F, OUTPUT); pinMode(ECHO_F, INPUT);
  pinMode(TRIG_R, OUTPUT); pinMode(ECHO_R, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  vibrate(2, 150);
  Serial.println("System Ready!");
}

void loop() {
  long left  = readSensor(TRIG_L, ECHO_L);
  long front = readSensor(TRIG_F, ECHO_F);
  long right = readSensor(TRIG_R, ECHO_R);

  Serial.print("বাম: "); Serial.print(left);
  Serial.print(" | সামনে: "); Serial.print(front);
  Serial.print(" | ডান: "); Serial.println(right);

  long closest = min(left, min(front, right));

  if (closest < CRITICAL_DIST) {
    // Critical Zone — Nonstop vibration
    digitalWrite(MOTOR_PIN, HIGH);
    mp3.playTrackNumber(4, 100, true); // বিপদ!
    delay(800);

  } else if (closest < DANGER_DIST) {
    // Danger Zone — দিক অনুযায়ী pulse
    if (left == closest) {
      vibrate(1, 200);              // ১ বার
      mp3.playTrackNumber(1, 100, true); // বাম
      delay(800);
    }
    else if (front == closest) {
      vibrate(2, 200);              // ২ বার
      mp3.playTrackNumber(2, 100, true); // সামনে
      delay(800);
    }
    else if (right == closest) {
      vibrate(3, 200);              // ৩ বার 
      mp3.playTrackNumber(3, 100, true); // ডান
      delay(800);
    }

  } else {
    // Safe Zone — motor বন্ধ
    digitalWrite(MOTOR_PIN, LOW);
  }

  delay(100);
} 