const int sensorPin = A0; // 조도 센서의 핀 번호

void setup() {
  Serial.begin(9600); // 시리얼 통신 시작
}

void loop() {
  int sensorValue = analogRead(sensorPin); // 조도 센서의 값 읽기
  Serial.print("Light Sensor Value: "); // 시리얼 모니터에 출력
  Serial.println(sensorValue); // 시리얼 모니터에 조도 센서 값 출력
  delay(1000); // 1초 대기
}