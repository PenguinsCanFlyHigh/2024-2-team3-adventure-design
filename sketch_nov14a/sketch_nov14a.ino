#include <SD.h>
#include <Wire.h>
#include <OV7670.h> // OV7670 라이브러리가 필요합니다.

const int CS_PIN = 10; // SD 카드 모듈의 CS 핀 연결

OV7670 cam; // 카메라 객체 생성

void setup() {
    Serial.begin(9600);

    // SD 카드 초기화
    if (!SD.begin(CS_PIN)) {
        Serial.println("SD 카드 초기화 실패!");
        return;
    }
    Serial.println("SD 카드 초기화 성공.");

    // OV7670 카메라 초기화
    if (!cam.init()) {
        Serial.println("카메라 초기화 실패!");
        return;
    }
    Serial.println("카메라 초기화 성공.");

    // 초기 설정
    cam.setResolution(QQVGA); // 해상도 설정
    cam.setColorSpace(RGB565); // 컬러 스페이스 설정
}

void loop() {
    delay(5000); // 5초마다 촬영
    
    // 이미지 캡처
    cam.capture();

    // 파일 생성 및 저장
    File imgFile = SD.open("image.jpg", FILE_WRITE);
    if (imgFile) {
        cam.saveImage(imgFile); // 이미지 데이터 저장
        imgFile.close();
        Serial.println("이미지 저장 완료.");
    } else {
        Serial.println("이미지 저장 실패!");
    }
}