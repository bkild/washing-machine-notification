Sameple md파일

# JSON 데이터 포맷 정의

## 1. ESP32 → 서버 (세탁기 상태 전송)

```json
{
  "device_id": "esp32_washer_01",
  "status": "running",    // running, completed, idle
  "time_left": "25",        // 남은 세탁 시간 (분 단위)
  "alert": "false",         // 세탁 완료 알림 여부
}
```
2. 서버 → 클라이언트 (세탁 상태 브로드캐스트)
json

```json
{
  "type": "washer_status",
  "data": {
    "status": "completed",
    "time_left": "0",
    "alert": "true",
  }
}
```
3. 클라이언트 → 서버 (알람 중지 요청)
```json
{
  "device_id": "client_mobile_01",
  "action": "stop_alert",
}
```