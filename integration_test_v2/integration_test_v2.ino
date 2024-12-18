// 이지플랜팅 키트 기능 통합: 조명 제어, 토양 수분 체크, 온습도 안내 장치
// 사용 모듈: 조도 센서, 릴레이 모듈, 빗물 감지 센서, LED, 온습도 센서 (DHT11), I2C LCD 디스플레이, RTC 모듈, 팬 모터

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DS1302.h>

// --- 조도 제어 관련 설정 ---
#define LIGHT_SENSOR_PIN A3  // 조도 센서 연결 핀
#define RELAY_PIN 33         // 릴레이 모듈 핀 (조명 제어용)

// --- 토양 수분 체크 관련 설정 ---
#define RAIN_SENSOR_ANALOG_PIN A8 // 빗물 감지 센서의 아날로그 핀 번호
#define RAIN_SENSOR_DIGITAL_PIN 37  // 빗물 감지 센서의 디지털 핀 번호
#define LED_PIN 9                 // 빗물이 많이 감지되면 켜질 LED의 핀 번호
#define LED_PIN2 8                // 빗물이 조금 감지되면 켜질 LED의 핀 번호
#define LED_PIN3 7                // 빗물이 감지 안되면 켜질 LED의 핀 번호

// --- 온습도 안내 장치 관련 설정 ---
#define DHTPIN 27        // 온습도 센서 핀 (DHT 데이터 핀)
#define DHTTYPE DHT11   // DHT11 타입 센서 사용

// --- 팬 모터 제어 ---
#define FAN_PIN 10       // 팬 모터 제어 핀

// --- RTC 모듈 설정 ---
#define CLK_PIN 26  // RTC 클럭 핀
#define DAT_PIN 25  // RTC 데이터 핀
#define RST_PIN 24  // RTC 리셋 핀
DS1302 rtc(CLK_PIN, DAT_PIN, RST_PIN);

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

  // --- 팬 모터 초기화 ---
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW); // 기본적으로 팬 모터 OFF 상태

  // --- RTC 초기화 ---
  rtc.halt(false);
  rtc.writeProtect(false);

  // 필요 시 초기 시간 설정 (한번만 설정 후 주석 처리)
  rtc.setDOW(MONDAY);  // 요일 설정
  rtc.setTime(12, 00, 0); // 시:분:초 설정
  rtc.setDate(9, 12, 2024); // 일, 월, 년 설정

  Serial.println("RTC Initialized");
}

void loop() {
  // --- 현재 시간 확인 ---
  Time currentTime = rtc.getTime();
  int currentHour = currentTime.hour;
  int currentMinute = currentTime.min;

  Serial.print("Current Time: ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.println(currentMinute);

  // --- 조도 제어 기능 ---
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Light Value: ");
  Serial.println(lightValue);

  if (currentHour >= 6 && currentHour < 18) {  // 주간 시간대 (06:00 ~ 18:00)
    if (lightValue > 500) {  // 조도 값이 낮을 경우 (예: 500 초과)
      digitalWrite(RELAY_PIN, HIGH);  // 조명 ON
      Serial.println("Light ON");
    } else {
      digitalWrite(RELAY_PIN, LOW);   // 조명 OFF
      Serial.println("Light OFF");
    }
  } else {  // 야간 시간대 (18:00 ~ 06:00)
    digitalWrite(RELAY_PIN, LOW);     // 조명 OFF 유지
    Serial.println("Night Time: Light OFF");
  }

  // --- 토양 수분 체크 기능 ---
  int rainSensorAnalogValue = analogRead(RAIN_SENSOR_ANALOG_PIN);
  Serial.print("토양 수분 수치: ");
  Serial.println(rainSensorAnalogValue);

  if (rainSensorAnalogValue < 100) {
    Serial.println("수분 많이 감지됨!");
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);
  } else if (rainSensorAnalogValue < 300) {
    Serial.println("수분 조금 감지됨!");
    digitalWrite(LED_PIN2, HIGH);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN3, LOW);
  } else {
    Serial.println("수분 감지안됨!");
    digitalWrite(LED_PIN3, HIGH);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN2, LOW);
  }

  // --- 온습도 안내 장치 및 팬 제어 ---
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

    // 팬 모터 제어
    if (temperature >= 20) {  // 온도가 20도 이상일 때
      digitalWrite(FAN_PIN, HIGH);  // 팬 ON
      Serial.println("Fan ON");
    } else {
      digitalWrite(FAN_PIN, LOW);  // 팬 OFF
      Serial.println("Fan OFF");
    }
  }

  delay(2000); // 2초 대기 후 업데이트
}
