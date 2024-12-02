#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// SD 카드 CS 핀 설정 (보드에 따라 다를 수 있음)
const int chipSelect = 4;

void setup() {
  Serial.begin(115200); // 시리얼 모니터 시작
  Wire.begin(); // I2C 초기화

  // SD 카드 초기화
  if (!SD.begin(chipSelect)) {
    Serial.println("SD 카드 초기화 실패");
    while (true); // 무한 루프로 종료
  }
  Serial.println("SD 카드 초기화 완료");

  // 카메라 초기화
  initializeCamera();
}

void loop() {
  captureImage(); // 이미지 캡처 함수 호출
  delay(5000); // 5초 대기 후 다시 촬영
}

void initializeCamera() {
  Wire.beginTransmission(0x42); // OV7670 I2C 주소 (확인 필요)
  Wire.write(0x12); // 소프트 리셋 명령
  Wire.write(0x80);
  Wire.endTransmission();
  delay(100); // 초기화 지연

  Serial.println("카메라 초기화 완료");
}

void captureImage() {
  Serial.println("사진 촬영 중...");

  File imgFile = SD.open("image.txt", FILE_WRITE);
  if (imgFile) {
    // 더미 데이터 저장 (OV7670 출력 시퀀스 설정 필요)
    for (int i = 0; i < 1024; i++) {
      byte pixelData = random(0, 255); // 실제로는 카메라 데이터 읽어야 함
      imgFile.write(pixelData);
    }
    imgFile.close();
    Serial.println("이미지 저장 완료");
  } else {
    Serial.println("이미지 파일을 열 수 없습니다.");
  }
}
