#include "DHT.h"  // DHT 센서를 사용하기 위한 라이브러리 포함
#include <Wire.h>  // I2C 통신을 위한 Wire 라이브러리 포함
#include <LiquidCrystal_I2C.h>  // I2C 방식의 LCD를 사용하기 위한 라이브러리 포함

#define DHTPIN 7     // DHT 센서가 연결된 핀 번호를 정의
#define DHTTYPE DHT22   // 사용된 DHT 센서의 종류(DHT22)를 정의, DHT11의 경우 DHT11로 변경 필요

DHT dht(DHTPIN, DHTTYPE);  // DHT 객체 생성 및 핀 번호와 센서 종류 지정
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD 객체 생성, I2C 주소 0x27, 16x2 크기의 LCD 사용

void setup() {
  Serial.begin(9600);  // 시리얼 통신을 9600bps로 시작
  Serial.println(F("DHTxx 테스트 시작!"));  // 시리얼 모니터에 테스트 시작 메시지 출력
  pinMode(10, OUTPUT); // 모터 핀 출력 설정

  dht.begin();  // DHT 센서 초기화
  lcd.begin();  // LCD 초기화
  lcd.backlight();  // LCD 백라이트 켜기
  lcd.print("시작 중...");  // LCD에 시작 메시지 출력
  delay(2000);  // 2초 대기
}

void loop() {
  delay(2000);  // 2초 간격으로 업데이트

  float h = dht.readHumidity();  // DHT 센서로부터 습도 값 읽기
  float t = dht.readTemperature();  // DHT 센서로부터 섭씨 온도 값 읽기

  if (isnan(h) || isnan(t)) {  // 만약 습도나 온도 값이 올바르게 읽히지 않으면
    Serial.println(F("DHT 센서로부터 읽기 실패!"));  // 시리얼 모니터에 오류 메시지 출력
    lcd.clear();  // LCD 화면 지우기
    lcd.print("센서 오류");  // LCD에 오류 메시지 출력
    return;  // 함수 종료
  }

  float hif = dht.computeHeatIndex(t, h, false);  // 섭씨 온도와 습도를 이용하여 열지수 계산

  // 시리얼 모니터에 측정된 값 출력
  Serial.print(F("습도: "));
  Serial.print(h);  // 습도 값 출력
  Serial.print(F("%  온도: "));
  Serial.print(t);  // 온도 값 출력
  Serial.print(F("°C  열지수: "));
  Serial.print(hif);  // 열지수 값 출력
  Serial.println(F("°C"));

  // LCD에 측정된 값 출력
  lcd.clear();  // LCD 화면 지우기
  lcd.setCursor(0, 0);  // LCD 커서를 첫 번째 줄 첫 번째 칸으로 이동
  lcd.print("온도: ");  // "온도: " 출력
  lcd.print(t);  // 온도 값 출력
  lcd.print(" C");  // 섭씨 단위 출력
  lcd.setCursor(0, 1);  // LCD 커서를 두 번째 줄 첫 번째 칸으로 이동
  lcd.print("습도: ");  // "습도: " 출력
  lcd.print(h);  // 습도 값 출력
  lcd.print(" %");  // 퍼센트 단위 출력

  //  일정 온도가 넘으면 모터 작동
  if(t >= 25) // 온도가 25 이상일 때
  {
    digitalWrite(10, HIGH); // 모터 돌림
  }
  else // 온도가 25 이하일 때
  {
    digitalWrite(10, LOW); // 모터 안 돌림
  }

  delay(2000); // 딜레이 2초
}