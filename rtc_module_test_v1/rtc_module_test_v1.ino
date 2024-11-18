#include <Wire.h>
#include <DS1302.h>

// DS1302 핀 설정
#define CLK_PIN 26  // 클럭 핀
#define DAT_PIN 25  // 데이터 핀
#define RST_PIN 24  // 리셋 핀

// DS1302 객체 생성
DS1302 rtc(CLK_PIN, DAT_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  Serial.println("RTC Simple Test");

  // // RTC 시간 설정 (초기 설정 실행 후 주석 처리하기!)
  rtc.halt(false);
  rtc.writeProtect(false);
  rtc.setDOW(MONDAY);  // 요일 설정
  rtc.setTime(11, 32, 0); // 시:분:초 설정
  rtc.setDate(18, 11, 2024); // 일, 월, 년 설정

  delay(3000);

  // 설정된 시간 확인
  Time currentTime = rtc.getTime();
  Serial.print("Set Time: ");
  Serial.print(currentTime.hour);
  Serial.print(":");
  Serial.print(currentTime.min);
  Serial.print(":");
  Serial.println(currentTime.sec);

  Serial.println("RTC Initialized");
}

void loop() {
  Time currentTime = rtc.getTime();

  // 유효성 검증을 통해 잘못된 데이터를 무시하고 재시도
  if (currentTime.hour > 23 || currentTime.min > 59 || currentTime.sec > 59 ||
      currentTime.hour < 0 || currentTime.min < 0 || currentTime.sec < 0) {
    Serial.println("Invalid time detected. Retrying...");
    delay(1000);
    return; // 해당 루프를 종료하고 다시 읽도록 설정
  }

  Serial.print("Current Time: ");
  Serial.print(currentTime.hour);
  Serial.print(":");
  Serial.print(currentTime.min);
  Serial.print(":");
  Serial.println(currentTime.sec);

  Serial.print("Day of Week: ");
  Serial.println(currentTime.dow);

  delay(1000); // 1초마다 확인
}
