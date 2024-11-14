// 이지플랜팅 키트 기능 통합: 조명 제어, 토양 수분 체크, 온습도 안내 장치
// 사용 모듈: 조도 센서, 릴레이 모듈, 빗물 감지 센서, LED, 온습도 센서 (DHT11), I2C LCD 디스플레이

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- 조도 제어 관련 설정 ---
#define LIGHT_SENSOR_PIN A3  // 조도 센서 연결 핀
#define RELAY_PIN 33          // 릴레이 모듈 핀 (조명 제어용)

// --- 토양 수분 체크 관련 설정 ---
#define RAIN_SENSOR_ANALOG_PIN A8 // 빗물 감지 센서의 아날로그 핀 번호
#define RAIN_SENSOR_DIGITAL_PIN 37  // 빗물 감지 센서의 디지털 핀 번호
#define LED_PIN 9                 // 빗물이 많이 감지되면 켜질 LED의 핀 번호
#define LED_PIN2 8                // 빗물이 조금 감지되면 켜질 LED의 핀 번호
#define LED_PIN3 7                // 빗물이 감지 안되면 켜질 LED의 핀 번호

// --- 온습도 안내 장치 관련 설정 ---
#define DHTPIN 27        // 온습도 센서 핀 (DHT 데이터 핀)
#define DHTTYPE DHT11   // DHT11 타입 센서 사용

// 객체 생성
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C 주소가 0x27인 LCD (16x2)

void setup() {
  // --- 공통 초기화 ---
  Serial.begin(9600);
  Serial.println("Easy Planting Kit - Combined Setup");

  // --- 조도 제어 초기화 ---
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // 기본적으로 조명 OFF 상태로 설정
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  // --- 토양 수분 체크 초기화 ---
  pinMode(RAIN_SENSOR_DIGITAL_PIN, INPUT); // 디지털 핀을 입력으로 설정
  pinMode(LED_PIN, OUTPUT);                // LED 핀을 출력으로 설정
  pinMode(LED_PIN2, OUTPUT);               // LED 핀을 출력으로 설정
  pinMode(LED_PIN3, OUTPUT);               // LED 핀을 출력으로 설정

  // --- 온습도 안내 장치 초기화 ---
  lcd.begin();
  lcd.backlight();
  lcd.print("Initializing...");
  dht.begin();
  delay(2000); // 초기화 후 대기
  lcd.clear();
}

void loop() {
  // --- 조도 제어 기능 ---
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Light Value: ");
  Serial.println(lightValue);

  if (lightValue > 500) {  // 조도 값이 낮을 경우 (예: 500 미만)
    digitalWrite(RELAY_PIN, HIGH);  // 조명 ON
    Serial.println("Light ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);   // 조명 OFF
    Serial.println("Light OFF");
  }

  // --- 토양 수분 체크 기능 ---
  int rainSensorAnalogValue = analogRead(RAIN_SENSOR_ANALOG_PIN);
  Serial.print("아날로그 값: ");
  Serial.println(rainSensorAnalogValue); // 아날로그 값 시리얼 모니터에 출력

  if (rainSensorAnalogValue < 400) {
    Serial.println("빗물 많이 감지됨!");
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);
  } else if (rainSensorAnalogValue < 800) {
    Serial.println("빗물 조금 감지됨!");
    digitalWrite(LED_PIN2, HIGH);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN3, LOW);
  } else {
    Serial.println("빗물 감지안됨!");
    digitalWrite(LED_PIN3, HIGH);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN2, LOW);
  }

  // --- 온습도 안내 장치 기능 ---
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    lcd.clear();
    lcd.print("Sensor Error");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print(" %");
  }

  delay(2000); // 2초 대기 후 업데이트
} 
