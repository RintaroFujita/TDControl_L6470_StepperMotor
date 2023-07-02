#include <SPI.h>

#define L6470_SS_PIN 10
#define L6470_SS_PIN2 8

void setup() {
  SPI.begin();
  pinMode(L6470_SS_PIN, OUTPUT);
  digitalWrite(L6470_SS_PIN, HIGH);
  pinMode(L6470_SS_PIN2, OUTPUT);
  digitalWrite(L6470_SS_PIN2, HIGH);

  // L6470の設定
  L6470_setup();
  L6470_setup2();

  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    // シリアルからデータを取得
    int flag = Serial.parseInt(); // シリアルから整数値を読み取る

    if (flag == 1) {
      rotateMotorMicrostep(1, 1, 500); // 1台目は正転, 2台目は正転
    } else if (flag == 2) {
      rotateMotorMicrostep(-1, -1, 500); // 1台目は逆転, 2台目は逆転
    } else if (flag == 3) {
      rotateMotorMicrostep(1, -1, 500); // 1台目は正転, 2台目は逆転
    } else if (flag == 4) {
      rotateMotorMicrostep(-1, 1, 500); // 1台目は逆転, 2台目は正転
    } else {
      stopMotor(); // モーターを停止
    }
  }
}

void L6470_send(unsigned char value, int ssPin) {
  digitalWrite(ssPin, LOW);
  SPI.transfer(value);
  digitalWrite(ssPin, HIGH);
}

void L6470_setup() {
  // コンフィギュレーションレジスタへの設定
  L6470_send(0x0B, L6470_SS_PIN); // コマンドビット(0x0B)でアクセスを要求
  L6470_send(0x0D, L6470_SS_PIN); // コンフィギュレーションレジスタに値を設定し書き込み（マイクロステップ）
}

void L6470_setup2() {
  // 2台目のL6470の設定
  L6470_send(0x0B, L6470_SS_PIN2); // コマンドビット(0x0B)でアクセスを要求
  L6470_send(0x0D, L6470_SS_PIN2); // コンフィギュレーションレジスタに値を設定し書き込み（マイクロステップ）
}

void rotateMotorMicrostep(float kaiten1, float kaiten2, int speed) {
  // 回転方向と速度
  if (kaiten1 > 0) {
    L6470_send(0x50, L6470_SS_PIN); // 1台目は正転
  } else {
    L6470_send(0x51, L6470_SS_PIN); // 1台目は逆転
  }

  if (kaiten2 > 0) {
    L6470_send(0x50, L6470_SS_PIN2); // 2台目は正転
  } else {
    L6470_send(0x51, L6470_SS_PIN2); // 2台目は逆転
  }

  // 回転速度を設定する
  speed = constrain(speed, 0, 1023);
  byte speedHigh = speed / 256;
  byte speedLow = speed % 256;
  L6470_send(speedHigh, L6470_SS_PIN);
  L6470_send(speedLow, L6470_SS_PIN);
  L6470_send(speedHigh, L6470_SS_PIN2);
  L6470_send(speedLow, L6470_SS_PIN2);

  // 指定した回転数だけ回転させる
  int steps1 = abs(kaiten1) * 128;
  int steps2 = abs(kaiten2) * 128;

  for (int i = 0; i < max(steps1, steps2); i++) {
    if (i < steps1) {
      L6470_send(0x55, L6470_SS_PIN); // 1台目のステップクロックパルスを送信
    }
    if (i < steps2) {
      L6470_send(0x55, L6470_SS_PIN2); // 2台目のステップクロックパルスを送信
    }
    delay(1); // 1ミリ秒の遅延
  }
}

void stopMotor() {
  L6470_send(0xB8, L6470_SS_PIN2); // 2台目のモーターを停止(デイジーチェーンの関係で急に止まりにくいのでハードストップ0xB8を利用)
  L6470_send(0xB8, L6470_SS_PIN); // 1台目のモーターを停止(ハードストップ0xB8)
}
