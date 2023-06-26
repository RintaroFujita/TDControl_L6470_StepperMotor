#include <SPI.h>

#define L6470_SS_PIN 10

int rotationCount = 0;
int currentRotationCount = 0;
bool isMotorRunning = false;

void setup() {
  SPI.begin();
  pinMode(L6470_SS_PIN, OUTPUT);
  digitalWrite(L6470_SS_PIN, HIGH);

  // L6470の設定
  L6470_setup();

  Serial.begin(9600);
  Serial.setTimeout(1); // シリアルタイムアウトを1ミリ秒に設定
}

void loop() {
  if (Serial.available() > 0) {
    // シリアルからデータを取得
    rotationCount = Serial.parseInt(); // シリアルから整数値を読み取る

    if (rotationCount != currentRotationCount) {
      currentRotationCount = rotationCount;
      if (rotationCount >= 0 && rotationCount <= 9) {
        rotateMotorMicrostep(1, map(rotationCount, 0, 9, 0, 200)); // モーターが指定した回転数だけ正転し、回転速度が指定された数値で回る
        isMotorRunning = true;
      } else {
        stopMotor(); // モーターを停止
        isMotorRunning = false;
      }
    }
  }
if (isMotorRunning) {
    Serial.println(currentRotationCount);
  }
}

void L6470_send(unsigned char value) {
  digitalWrite(L6470_SS_PIN, LOW);
  SPI.transfer(value);
  digitalWrite(L6470_SS_PIN, HIGH);
}

void L6470_setup() {
  // コンフィギュレーションレジスタへの設定
  L6470_send(0x0B); // コマンドビット(0x0B)でアクセスを要求
  L6470_send(0x0D); // コンフィギュレーションレジスタに値を設定し書き込み（マイクロステップ）
}

void rotateMotorMicrostep(float kaiten, int count) {
  L6470_send(0x50); // 正転

  // 回転速度を設定する
  byte speedHigh = count / 256;
  byte speedLow = count % 256;
  L6470_send(speedHigh);
  L6470_send(speedLow);
}

void stopMotor() {
  L6470_send(0xB0); // モーターを停止
}

int getCurrentRotationCount() {
  int steps = currentRotationCount * 128;
  int currentCount = steps / 128; // 1回転あたりのステップ数
  return currentCount;
}
