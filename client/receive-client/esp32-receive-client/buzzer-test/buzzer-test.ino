/*
 * 아두이노 부저음: 반짝반짝 작은 별 (Twinkle, Twinkle, Little Star)
 * 세탁기 완료 알림음으로 사용하기 적합합니다.
 */

// 부저가 연결된 디지털 핀 번호 정의
const int buzzerPin = 21;

// 음계 주파수 (Frequency) 정의
// 중학교 음악 시간에 배우는 '도레미파솔라시'는 4옥타브입니다.
#define NOTE_C4 262   // 도 (C)
#define NOTE_D4 294   // 레 (D)
#define NOTE_E4 330   // 미 (E)
#define NOTE_F4 349   // 파 (F)
#define NOTE_G4 392   // 솔 (G)
#define NOTE_A4 440   // 라 (A)
#define NOTE_B4 494   // 시 (B)
#define NOTE_C5 523   // 높은 도 (High C)
#define REST      0     // 쉼표 (Rest)

// 멜로디 (음계 배열)
// '반짝반짝 작은 별' 전체 곡입니다.
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, // 반짝반짝 작은 별
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, // 아름답게 비치네
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, // 서쪽 하늘에서도
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, // 동쪽 하늘에서도
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, // 반짝반짝 작은 별
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4  // 아름답게 비치네
};

// 박자 (Duration 배열)
// 4는 4분음표, 2는 2분음표 (길게)를 의미합니다.
// 멜로디 배열의 음표 개수와 동일해야 합니다.
int noteDurations[] = {
  4, 4, 4, 4, 4, 4, 2, // 7개
  4, 4, 4, 4, 4, 4, 2, // 7개
  4, 4, 4, 4, 4, 4, 2, // 7개
  4, 4, 4, 4, 4, 4, 2, // 7개
  4, 4, 4, 4, 4, 4, 2, // 7개
  4, 4, 4, 4, 4, 4, 2  // 7개 (총 42개)
};

// 템포 조정 (숫자가 작을수록 빨라짐)
const int tempo = 60; // 1분당 120박자의 일반적인 속도

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // 전체 멜로디의 길이를 계산합니다.
  int notes = sizeof(melody) / sizeof(int);

  // 멜로디를 순서대로 연주합니다.
  for (int i = 0; i < notes; i++) {
    // 박자(1/4, 1/8 등)를 밀리초(ms)로 변환합니다.
    // (60000 ms/분 / 템포) / 박자 = 음표 지속 시간
    int duration = 60000 / tempo / noteDurations[i];
    
    // tone(핀, 주파수, 지속 시간) 함수로 소리를 냅니다.
    tone(buzzerPin, melody[i], duration);
    
    // 다음 음을 연주하기 전에 약간의 쉼표를 줍니다.
    // 음표 지속 시간의 30%만큼 쉬어줍니다.
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    
    // 소리를 멈춥니다.
    noTone(buzzerPin);
  }

  // 멜로디 연주가 끝난 후, 세탁기 완료 알림이라면 
  // 여기서는 한 번만 울리게 하거나, 
  // 아래와 같이 완료 후 대기 시간을 둡니다.

  // 세탁기 완료 알림이라면 loop()를 빠져나와 한번만 울리거나, 
  // 지속적으로 알림을 원하면 delay 시간을 조절합니다.
  delay(5000); // 5초 대기 후 다시 반복 (세탁기 완료 알림이라면 이 부분을 제거하거나, delay(9999999)로 긴 대기를 줍니다)
}