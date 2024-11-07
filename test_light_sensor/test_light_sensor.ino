#define RELAY_PIN 33 // 릴레이에 연결된 핀 번호

void setup() {
  pinMode(RELAY_PIN, OUTPUT); // 릴레이 핀을 출력으로 설정
}

void loop() {
  digitalWrite(RELAY_PIN, HIGH); // 릴레이 켜기
  delay(5000); // 5초 대기
  digitalWrite(RELAY_PIN, LOW); // 릴레이 끄기
  delay(5000); // 5초 대기
}