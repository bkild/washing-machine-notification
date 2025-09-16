GTP가 작성한 샘플 md  

DB: SQLite (라즈베리파이4에 로컬 저장)

## 1. devices 테이블 (연결된 장치 관리)
| 필드명      | 타입       | 설명                   |
|-------------|------------|------------------------|
| id          | INTEGER PK | 고유 ID (자동 증가)    |
| device_id   | TEXT       | 장치 식별자            |
| type        | TEXT       | esp32 / client / etc   |
| last_seen   | DATETIME   | 마지막 통신 시각       |

---

## 2. washer_status 테이블 (세탁 상태 로그)
| 필드명      | 타입       | 설명                   |
|-------------|------------|------------------------|
| id          | INTEGER PK | 고유 ID (자동 증가)    |
| device_id   | TEXT       | ESP32 장치 식별자      |
| status      | TEXT       | running / completed    |
| time_left   | INTEGER    | 남은 세탁 시간(분)     |
| alert       | BOOLEAN    | 알람 여부              |
| timestamp   | DATETIME   | 기록 시각              |

---

## 3. alerts 테이블 (알람 기록)
| 필드명      | 타입       | 설명                     |
|-------------|------------|--------------------------|
| id          | INTEGER PK | 고유 ID                  |
| device_id   | TEXT       | 알람 요청한 클라이언트ID |
| action      | TEXT       | stop_alert 등            |
| timestamp   | DATETIME   | 요청 시각                |

---

✅ 기본적으로 **washer_status** 는 로그성 데이터 → 주기적으로 쌓이고  
✅ **devices** 는 장치 관리, **alerts** 는 사용자 액션 관리용