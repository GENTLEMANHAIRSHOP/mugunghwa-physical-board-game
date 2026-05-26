# System Architecture

## 전체 구조

이 프로젝트는 센서 입력, 게임 로직, 출력 장치를 하나로 연결한 피지컬 컴퓨팅 보드게임입니다.

```txt
Player Action
      ↓
Movement Sensor / Button
      ↓
Arduino Uno
      ↓
Game State Logic
      ↓
Servo Motor / LED / Buzzer
      ↓
Game Result
```
