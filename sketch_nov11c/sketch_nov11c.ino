#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// DHT 설정
#define DHTPIN 2 // DHT 데이터 핀
#define DHTTYPE DHT11 // DHT11 또는 DHT22 중 사용 중인 모델에 맞게 설정
DHT dht(DHTPIN, DHTTYPE);

// LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C 주소가 0x27인 경우 (다를 수 있음)

// 초기 설정
void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin();
  lcd.backlight(); // 백라이트 켜기
}

// 메인 루프
void loop() {
  // 온도와 습도 읽기
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // 데이터 유효성 검사
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("센서에서 데이터를 읽을 수 없습니다!");
    lcd.setCursor(0, 0);
    lcd.print("Read Error!");
    return;
  }

  // LCD에 온습도 표시
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");

  delay(2000); // 2초마다 갱신
}
