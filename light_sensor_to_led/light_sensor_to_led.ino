// 이지플랜팅 키트 1번 기능: 조도에 따라 자동으로 On/Off 되는 조명장치
// 사용 모듈: 조도 센서, 릴레이 모듈

#include <Wire.h>

#define LIGHT_SENSOR_PIN A3  // 조도 센서 연결 핀
#define RELAY_PIN 33          // 릴레이 모듈 핀 (조명 제어용)

void setup() {
  // 릴레이 핀 초기화
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // 기본적으로 조명 OFF 상태로 설정

  // 조도 센서 핀 초기화
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  // 시리얼 모니터 시작 (디버깅 용도)
  Serial.begin(9600);
}

void loop() {
  // 조도 값 읽기
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Light Value: ");
  Serial.println(lightValue);

  // 조도에 따른 조명 제어
  if (lightValue > 500) {  // 조도 값이 낮을 경우 (예: 500 미만)
    digitalWrite(RELAY_PIN, HIGH);  // 조명 ON
    Serial.println("Light ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);   // 조명 OFF
    Serial.println("Light OFF");
  }

  delay(1000);  // 1초마다 값 확인
}