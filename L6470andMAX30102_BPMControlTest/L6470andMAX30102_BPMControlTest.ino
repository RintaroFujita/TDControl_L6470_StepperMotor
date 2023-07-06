#include <SPI.h>
#include <Wire.h>

#define L6470_SS_PIN 10

const int MAX_SAMPLES = 200; // 平均化するサンプルの最大数
const int BPM_THRESHOLD = 50; // モーターを回転させるための最小BPM閾値
const int BPM_TARGET = 60; // モーターの速度制御のための目標BPM

unsigned long samples[MAX_SAMPLES];
int sampleIndex = 0;
unsigned long lastTime = 0;
int bpm = 0;

void setup() {
  SPI.begin();
  pinMode(L6470_SS_PIN, OUTPUT);
  digitalWrite(L6470_SS_PIN, HIGH);

  // L6470の設定
  L6470_setup();

  Serial.begin(9600);
}

void loop() {
  unsigned long currentTime = millis();

  // サンプル時間を記録し、時間差を計算
  unsigned long timeDiff = currentTime - lastTime;
  lastTime = currentTime;

  // 時間差からBPMを計算
  int currentBPM = 60000 / timeDiff;

  // BPMをサンプル配列に格納
  samples[sampleIndex++] = currentBPM;
  sampleIndex %= MAX_SAMPLES;

  // 平均BPMを計算
  long sum = 0;
  for (int i = 0; i < MAX_SAMPLES; i++) {
    sum += samples[i];
  }
  bpm = sum / MAX_SAMPLES;

  Serial.print("BPM: ");
  Serial.println(bpm);

  // BPMに基づいてモーターを回転
  if (bpm > BPM_THRESHOLD) {
    float rotations = (float)(bpm - BPM_TARGET) / 60.0; // 必要な回転数を計算

    // モーターを指定の速度で回転させる
    rotateMotorMicrostep(rotations, 200);
  } else {
    // モーターを停止
    stopMotor();
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

void rotateMotorMicrostep(float rotations, int speed) {
  // 回転方向と速度
  if (rotations > 0) {
    L6470_send(0x50); // 正方向に回転するように設定。逆方向に回転する場合は0x51に変更する
  }

  // 回転速度を設定する
  speed = constrain(speed, 0, 1023);
  byte speedHigh = speed / 256;
  byte speedLow = speed % 256;
  L6470_send(speedHigh);
  L6470_send(speedLow);

  // 指定した回転数だけ回転させる
  int steps = abs(rotations) * 128;
  for (int i = 0; i < steps; i++) {
    L6470_send(0x55); // ステップクロックパルスを送信
  }
}

void stopMotor() {
  L6470_send(0xB0); // モーターを停止
}