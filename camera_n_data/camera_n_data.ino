#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>  // mDNS 라이브러리
#include "esp_camera.h"

// WiFi 설정 - 실제 네트워크 정보로 변경
const char* ssid = "WIFI_SSID";      // 연결할 WiFi SSID
const char* password = "WIFI_PASSWORD";  // WiFi 비밀번호
const char* mdnsName = "epstream";   // mDNS 이름 설정 (epstream.local)

// --- 핀 설정 (ESP32-CAM 모듈에 맞는 핀 설정) ---
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// --- 온습도 센서 설정 (DHT11/DHT22) ---
#define DHTPIN 27           // DHT 센서 데이터 핀 연결
#define DHTTYPE DHT11       // DHT 센서 타입 설정 (DHT11 또는 DHT22)
#include <DHT.h>
DHT dht(DHTPIN, DHTTYPE);

// --- 웹 서버 객체 생성 ---
WebServer server(8080);      // 포트 8080에서 웹 서버 시작

// --- 모니터링 데이터 변수 ---
float temperature = 0.0;     // 온도 저장 변수
float humidity = 0.0;        // 습도 저장 변수

void setup() {
  Serial.begin(115200);

  // 1. WiFi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi에 연결 중...");
  }
  Serial.println("WiFi 연결 완료");
  Serial.print("IP 주소: ");
  Serial.println(WiFi.localIP());

  // 2. mDNS 설정 (epstream.local로 접근 가능)
  if (!MDNS.begin(mdnsName)) {
    Serial.println("mDNS 설정 실패");
  } else {
    Serial.println("mDNS 설정 완료");
    Serial.printf("디바이스에 %s.local로 접근 가능\n", mdnsName);
  }

  // 3. DHT 센서 초기화
  dht.begin();
  Serial.println("DHT 센서 초기화 완료");

  // 4. 카메라 설정
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;  // XCLK 주파수
  config.pixel_format = PIXFORMAT_JPEG; // JPEG 포맷 설정
  config.frame_size = FRAMESIZE_VGA;    // 해상도 설정
  config.jpeg_quality = 10;             // JPEG 품질 설정 (0 ~ 63)
  config.fb_count = 2;                  // 프레임 버퍼 개수 설정

  // 카메라 초기화
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("카메라 초기화 실패: 오류 코드 %d\n", err);
    return;
  }
  Serial.println("카메라 초기화 완료");

  // 5. 웹 서버 엔드포인트 설정
  server.on("/stream", HTTP_GET, handleStream); // 스트리밍 URL 핸들러
  server.on("/data", HTTP_GET, handleData);     // 온습도 데이터 핸들러

  // 6. 웹 서버 시작
  server.begin();
  Serial.println("웹서버 시작");

  // mDNS 서비스 추가
  MDNS.addService("http", "tcp", 8080);  // HTTP 서비스 추가
}

void loop() {
  // 7. 온습도 데이터 주기적 갱신
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // 웹 서버 클라이언트 요청 처리
  server.handleClient();
  
  delay(2000); // 2초마다 데이터 갱신
}

// --- 비디오 스트리밍 핸들러 ---
void handleStream() {
  WiFiClient client = server.client();
  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  client.println();

  // 카메라 프레임을 지속적으로 전송
  while (client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get(); // 카메라 프레임 캡처
    if (!fb) break;

    client.print("--frame\r\n");
    client.print("Content-Type: image/jpeg\r\n");
    client.print("Content-Length: ");
    client.print(fb->len);
    client.print("\r\n\r\n");
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb); // 버퍼 반환
    delay(30); // 30ms 지연
  }
}

// --- 온습도 데이터 핸들러 ---
void handleData() {
  StaticJsonDocument<200> doc; // JSON 데이터 객체 생성
  doc["temperature"] = temperature; // 온도 추가
  doc["humidity"] = humidity;       // 습도 추가

  String jsonResponse;
  serializeJson(doc, jsonResponse); // JSON 데이터 직렬화

  server.send(200, "application/json", jsonResponse); // JSON 응답 반환
}
