#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>  // Add mDNS library
#include "esp_camera.h"

// WiFi 설정 - 실제 네트워크 정보로 변경
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const char* mdnsName = "epstream";  // mDNS 도메인 이름 설정

// 핀 설정
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

// 온습도 센서 연결 (DHT11/DHT22)
#define DHTPIN 27
#define DHTTYPE DHT11

#include <DHT.h>
DHT dht(DHTPIN, DHTTYPE);

WebServer server(8080);

// 모니터링 데이터 변수
float temperature = 0.0;
float humidity = 0.0;

void setup() {
  Serial.begin(115200);
  
  // WiFi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi에 연결 중...");
  }
  Serial.println("WiFi 연결 완료");
  Serial.print("IP 주소: ");
  Serial.println(WiFi.localIP());

  // mDNS 설정
  if (!MDNS.begin(mdnsName)) {
    Serial.println("mDNS 설정 실패");
  } else {
    Serial.println("mDNS 설정 완료");
    Serial.printf("디바이스에 %s.local로 접근 가능\n", mdnsName);
  }

  // DHT 센서 초기화
  dht.begin();

  // 카메라 설정 
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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // 카메라 초기화
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("카메라 초기화 실패: 오류 코드 %d\n", err);
    return;
  }

  // 웹서버 라우팅 설정
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/data", HTTP_GET, handleData);
  
  // 웹서버 시작
  server.begin();
  Serial.println("웹서버 시작");

  // mDNS 서비스 추가 (선택사항)
  MDNS.addService("http", "tcp", 8080);
}

void loop() {
  // 온습도 데이터 주기적 갱신
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // 웹서버 클라이언트 처리
  server.handleClient();
  
  delay(2000); // 2초마다 데이터 갱신
}

// 이전의 handleStream() 및 handleData() 함수는 동일하게 유지
void handleStream() {
  WiFiClient client = server.client();
  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  client.println();

  while (client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) break;

    client.print("--frame\r\n");
    client.print("Content-Type: image/jpeg\r\n");
    client.print("Content-Length: ");
    client.print(fb->len);
    client.print("\r\n\r\n");
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb);
    delay(30);
  }
}

void handleData() {
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;

  String jsonResponse;
  serializeJson(doc, jsonResponse);

  server.send(200, "application/json", jsonResponse);
}