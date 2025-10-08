#ifndef LITTLE_STAR
#define LITTLE_STAR
/*
 * 아두이노 부저음: 반짝반짝 작은 별 (Twinkle, Twinkle, Little Star)
 * 세탁기 완료 알림음으로 사용하기 적합합니다.
 */

// 부저가 연결된 디지털 핀 번호 정의
extern const int buzzerPin;

// 음계 주파수 (Frequency) 정의
// 중학교 음악 시간에 배우는 '도레미파솔라시'는 4옥타브입니다.
#define NOTE_C4 262  // 도 (C)
#define NOTE_D4 294  // 레 (D)
#define NOTE_E4 330  // 미 (E)
#define NOTE_F4 349  // 파 (F)
#define NOTE_G4 392  // 솔 (G)
#define NOTE_A4 440  // 라 (A)
#define NOTE_B4 494  // 시 (B)
#define NOTE_C5 523  // 높은 도 (High C)
#define REST 0       // 쉼표 (Rest)

// 멜로디 (음계 배열)
// '반짝반짝 작은 별' 전체 곡입니다.
extern int melody[];

// 박자 (Duration 배열)
// 4는 4분음표, 2는 2분음표 (길게)를 의미합니다.
// 멜로디 배열의 음표 개수와 동일해야 합니다.
extern int noteDurations[];

// 템포 조정 (숫자가 작을수록 빨라짐)
extern const int tempo;  // 1분당 120박자의 일반적인 속도
void little_star_init();
void little_star_play();
struct MelodyState {
  int index;
  unsigned long noteStart;
  int noteDuration;
  bool playing;
};
extern MelodyState melodyState;
void little_star_start();
void little_star_update();

#endif