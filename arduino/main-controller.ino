
#include <Servo.h>

/*
  Mugunghwa Physical Board Game
  Board: Arduino Uno

  무궁화 꽃이 피었습니다 피지컬 보드게임 테스트 코드입니다.

  기능:
  - 랜덤 시간 동안 Safe Time 진행
  - Freeze Time에는 움직임 감지 시 경고 증가
  - 경고 3회 시 게임 오버
  - 버튼을 눌러 움직임 감지 테스트
  - 서보모터로 말 이동 표현
  - LED와 부저로 상태 표시
*/

// 핀 설정
const int MOVE_SENSOR_PIN = 2;   // 움직임 감지 버튼 또는 센서
const int START_BUTTON_PIN = 3;  // 게임 시작 버튼
const int SAFE_LED_PIN = 8;      // 움직여도 되는 상태 LED
const int FREEZE_LED_PIN = 9;    // 멈춰야 하는 상태 LED
const int BUZZER_PIN = 10;       // 부저
const int SERVO_PIN = 11;        // 서보모터

Servo playerServo;

// 게임 상태
bool gameRunning = false;
bool freezeTime = false;

int warningCount = 0;
int playerPosition = 0;

// 시간 설정
unsigned long stateStartTime = 0;
unsigned long currentStateDuration = 0;

const int MAX_WARNING = 3;
const int SERVO_STEP = 15;
const int SERVO_MAX_ANGLE = 90;

void setup() {
  pinMode(MOVE_SENSOR_PIN, INPUT_PULLUP);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);

  pinMode(SAFE_LED_PIN, OUTPUT);
  pinMode(FREEZE_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  playerServo.attach(SERVO_PIN);
  playerServo.write(0);

  Serial.begin(9600);
  randomSeed(analogRead(A0));

  showIdleState();
  Serial.println("Mugunghwa Physical Board Game Ready");
}

void loop() {
  if (!gameRunning) {
    if (digitalRead(START_BUTTON_PIN) == LOW) {
      startGame();
      delay(300);
    }
    return;
  }

  unsigned long now = millis();

  if (now - stateStartTime >= currentStateDuration) {
    switchState();
  }

  if (freezeTime) {
    checkMovementDuringFreeze();
  }
}

void startGame() {
  gameRunning = true;
  freezeTime = false;
  warningCount = 0;
  playerPosition = 0;

  playerServo.write(playerPosition);

  Serial.println("Game Start!");
  beep(1);
  startSafeTime();
}

void startSafeTime() {
  freezeTime = false;
  stateStartTime = millis();
  currentStateDuration = random(3000, 7000);

  digitalWrite(SAFE_LED_PIN, HIGH);
  digitalWrite(FREEZE_LED_PIN, LOW);

  Serial.println("Safe Time: You can move!");

  movePlayerForward();
}

void startFreezeTime() {
  freezeTime = true;
  stateStartTime = millis();
  currentStateDuration = random(2000, 4000);

  digitalWrite(SAFE_LED_PIN, LOW);
  digitalWrite(FREEZE_LED_PIN, HIGH);

  Serial.println("Freeze Time: Stop!");
  beep(2);
}

void switchState() {
  if (freezeTime) {
    startSafeTime();
  } else {
    startFreezeTime();
  }
}

void checkMovementDuringFreeze() {
  if (digitalRead(MOVE_SENSOR_PIN) == LOW) {
    warningCount++;

    Serial.print("Movement Detected! Warning: ");
    Serial.println(warningCount);

    warningSignal();

    if (warningCount >= MAX_WARNING) {
      gameOver();
      return;
    }

    delay(700);
  }
}

void movePlayerForward() {
  playerPosition += SERVO_STEP;

  if (playerPosition >= SERVO_MAX_ANGLE) {
    playerPosition = SERVO_MAX_ANGLE;
    playerServo.write(playerPosition);
    gameClear();
    return;
  }

  playerServo.write(playerPosition);

  Serial.print("Player Position: ");
  Serial.println(playerPosition);
}

void gameClear() {
  Serial.println("Game Clear!");
  gameRunning = false;

  digitalWrite(SAFE_LED_PIN, HIGH);
  digitalWrite(FREEZE_LED_PIN, HIGH);

  successSignal();
}

void gameOver() {
  Serial.println("Game Over!");
  gameRunning = false;

  digitalWrite(SAFE_LED_PIN, LOW);
  digitalWrite(FREEZE_LED_PIN, HIGH);

  failSignal();
}

void showIdleState() {
  digitalWrite(SAFE_LED_PIN, LOW);
  digitalWrite(FREEZE_LED_PIN, LOW);
  playerServo.write(0);
}

void beep(int count) {
  for (int i = 0; i < count; i++) {
    tone(BUZZER_PIN, 1000, 150);
    delay(250);
  }
}

void warningSignal() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1500, 100);
    digitalWrite(FREEZE_LED_PIN, LOW);
    delay(100);
    digitalWrite(FREEZE_LED_PIN, HIGH);
    delay(100);
  }
}

void successSignal() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1200, 150);
    delay(200);
    tone(BUZZER_PIN, 1600, 150);
    delay(200);
  }
}

void failSignal() {
  for (int i = 0; i < 2; i++) {
    tone(BUZZER_PIN, 300, 400);
    delay(500);
  }
}
