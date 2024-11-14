#define RAIN_SENSOR_ANALOG_PIN A8 // 빗물 감지 센서의 아날로그 핀 번호
#define RAIN_SENSOR_DIGITAL_PIN 37  // 빗물 감지 센서의 디지털 핀 번호
#define LED_PIN 9                 // 빗물이 많이 감지되면 켜질 LED의 핀 번호
#define LED_PIN2 8                 // 빗물이 조금 감지되면 켜질 LED의 핀 번호
#define LED_PIN3 7                 // 빗물이 감지안되면 켜질 LED의 핀 번호
void setup() {
  pinMode(RAIN_SENSOR_DIGITAL_PIN, INPUT); // 디지털 핀을 입력으로 설정
  pinMode(LED_PIN, OUTPUT);                // LED 핀을 출력으로 설정
    pinMode(LED_PIN2, OUTPUT);                // LED 핀을 출력으로 설정
    pinMode(LED_PIN3, OUTPUT);                // LED 핀을 출력으로 설정
  Serial.begin(9600);                      // 시리얼 통신 시작
}

void loop() {
  int rainSensorAnalogValue = analogRead(RAIN_SENSOR_ANALOG_PIN); // 아날로그 핀을 통해 센서값 읽기
  int rainSensorDigitalState = digitalRead(RAIN_SENSOR_DIGITAL_PIN); // 디지털 핀을 통해 센서 상태 읽기

  Serial.print("아날로그 값: ");
  Serial.println(rainSensorAnalogValue); // 아날로그 값 시리얼 모니터에 출력

  if (rainSensorAnalogValue < 400) {
      Serial.println("빗물 많이 감지됨!"); // 시리얼 모니터에 메시지 출력
    digitalWrite(LED_PIN, HIGH);     // LED 켜기
    digitalWrite(LED_PIN2, LOW);    // LED 켜기
    digitalWrite(LED_PIN3, LOW);     // LED 켜기
  }

    else if (rainSensorAnalogValue < 800) {
      Serial.println("빗물 조금 감지됨!"); // 시리얼 모니터에 메시지 출력
    digitalWrite(LED_PIN2, HIGH);     // LED 켜기
    digitalWrite(LED_PIN, LOW);    // LED 켜기
    digitalWrite(LED_PIN3, LOW);     // LED 켜기
    }

    else if (rainSensorAnalogValue > 800) {
    Serial.println("빗물 감지안됨!"); // 시리얼 모니터에 메시지 출력
    digitalWrite(LED_PIN3, HIGH);    // LED 켜기
     digitalWrite(LED_PIN, LOW);     // LED 켜기
     digitalWrite(LED_PIN2, LOW);     // LED 켜기
    }

  delay(100); // 0.1초 대기
}
