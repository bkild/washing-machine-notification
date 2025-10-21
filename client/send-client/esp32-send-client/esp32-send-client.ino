#include "config.h"
#include "esp_camera.h"
#include <ArduinoJson.h>


#include <WiFi.h>
#include <ArduinoJson.h>

#include "seven_segment_led.h"

// ===========================
// Select camera model in board_config.h
// ===========================
#include "board_config.h"
String ssid = WIFI_SSID;
String password = WIFI_PASSWORD;
const char *host = SERVER_IP;
const uint16_t port = SERVER_PORT;
WiFiClient client;

void startCameraServer();
void setupLedFlash();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  seven_segment_init();

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  // config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;


  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_VGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

  // Setup LED FLash if LED pin is defined in camera_pins.h
  // #if defined(LED_GPIO_NUM)
  //   setupLedFlash();
  // #endif

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

String deviece_id = "esp32_washer_01";
String status="idle";
int time_left=8888;
bool alert = false;
void loop() {
  if (!client.connected()) {
    if (!client.connect(host, port)) {
      Serial.println("Connection to host failed");
      delay(1000);
      return;
    } else {
      Serial.println("Reconnected");
    }
  }

  // put your main code here, to run repeatedly:
  // 사진 캡처
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }


  // 이미지에서 시간 추출
  // Serial.printf("%d %d\n", fb->width, fb->height);
  int t = getTime(fb->buf, fb->len);
  if (t >= 0) {
    Serial.printf("%03d\n", t);
  } else {
    Serial.printf("Error\n");
  }

  if (t >= 0) {
    if (t == 0) {
      status = "completed";
      time_left = t;
      alert = true;
    } else {
      status = "running";
      time_left = t;
      alert = false;
    }
  }

  // JSON 시작
  JsonDocument doc;
  // Add values in the document
  //데이터 전송은 무조건 문자열로 통일
  doc["device_id"] = deviece_id;
  doc["status"] = status;                // running, completed, idle
  doc["time_left"] = String(time_left);  // 남은 세탁 시간 (분 단위)
  doc["alert"] = String(alert);          // 세탁 완료 알림 여부

  uint32_t length = measureJson(doc);
  // 네트워크 바이트 오더로 변환 (Big Endian)
  uint32_t length_net = htonl(length);

  // 4바이트 길이 먼저 전송
  client.write((uint8_t *)&length_net, sizeof(length_net));

  serializeJson(doc, client);
  // 메모리 반환
  esp_camera_fb_return(fb);

  delay(1000);
}
