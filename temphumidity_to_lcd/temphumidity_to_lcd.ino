// 이지플랜팅 키트 2번 기능: 온습도 안내 장치
// 사용 모듈: 온습도 센서 (DHT11), I2C LCD 디스플레이

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// 핀 설정
#define DHTPIN 27        // 온습도 센서 핀 (DHT 데이터 핀)
#define DHTTYPE DHT11   // DHT11 타입 센서 사용

// DHT 센서 및 LCD 객체 생성
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C 주소가 0x27인 LCD (16x2)

void setup() {
  // LCD 초기화
  lcd.begin();
  lcd.backlight();
  lcd.print("Initializing...");

  // DHT 센서 초기화
  dht.begin();

  delay(2000); // 초기화 후 대기
  lcd.clear();
}

void loop() {
  // 온도 및 습도 읽기
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // 센서 오류 확인
  if (isnan(humidity) || isnan(temperature)) {
    lcd.clear();
    lcd.print("Sensor Error");
  } else {
    // LCD에 온도와 습도 출력
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

  // 2초 대기 후 업데이트
  delay(2000);
}
