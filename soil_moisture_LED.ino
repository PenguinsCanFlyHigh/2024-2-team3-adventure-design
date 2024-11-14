#define RAIN_SENSOR_ANALOG_PIN A8  // 빗물 감지 센서의 아날로그 핀 번호
#define RAIN_SENSOR_DIGITAL_PIN 37  // 빗물 감지 센서의 디지털 핀 번호
#define RED_LED_PIN 5  // 빨간 LED 핀 번호
#define GREEN_LED_PIN 6  // 초록 LED 핀 번호
#define BLUE_LED_PIN 9  // 파란 LED 핀 번호

void setup() {
  pinMode(RAIN_SENSOR_DIGITAL_PIN, INPUT);  // 디지털 핀을 입력으로 설정
  pinMode(RED_LED_PIN, OUTPUT);  // 빨간 LED 핀을 출력으로 설정
  pinMode(GREEN_LED_PIN, OUTPUT);  // 초록 LED 핀을 출력으로 설정
  pinMode(BLUE_LED_PIN, OUTPUT);  // 파란 LED 핀을 출력으로 설정

  Serial.begin(9600);  // 시리얼 통신을 9600bps로 시작
}

void loop() {
  int rainSensorAnalogValue = analogRead(RAIN_SENSOR_ANALOG_PIN);  // 빗물 감지 센서 아날로그 값 읽기
  int rainSensorDigitalState = digitalRead(RAIN_SENSOR_DIGITAL_PIN);  // 빗물 감지 센서 디지털 상태 읽기

  Serial.print("빗물 감지 값: ");
  Serial.println(rainSensorAnalogValue);

  // 빗물 감지에 따른 LED 색상 변경
  if (rainSensorAnalogValue > 700) {  // 빗물 감지가 강할 때 (높은 값일수록 강한 감지)
    digitalWrite(GREEN_LED_PIN, HIGH);  // 초록 LED 켜기
    digitalWrite(BLUE_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  } else if (rainSensorAnalogValue > 400) {  // 빗물 감지가 중간일 때
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, HIGH);  // 파란 LED 켜기
    digitalWrite(RED_LED_PIN, LOW);
  } else {  // 빗물 감지가 약할 때
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);  // 빨간 LED 켜기
  }
  
  delay(100); // 0.1초 대기
}
