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
      rotateMotorMicrostep(2, 200); // モーターが1回転し、回転速度が200で回る
    } else if (flag == 2) {
      rotateMotorMicrostep(-2, 200); // モーターが逆回転して1回転し、回転速度が200で回る
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

void rotateMotorMicrostep(float kaiten, int speed) {
  // 回転方向と速度
  if (kaiten > 0) {
    L6470_send(0x50, L6470_SS_PIN); // 正転
    L6470_send(0x51, L6470_SS_PIN2); // 正転
  } else {
    L6470_send(0x51, L6470_SS_PIN); // 逆転
    L6470_send(0x50, L6470_SS_PIN2); // 逆転
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
  int steps = abs(kaiten) * 128;
  for (int i = 0; i < steps; i++) {
    L6470_send(0x55, L6470_SS_PIN); // ステップクロックパルスを送信
    L6470_send(0x55, L6470_SS_PIN2); // ステップクロックパルスを送信
  }
}

void stopMotor() {
  L6470_send(0xB0, L6470_SS_PIN); // モーターを停止
  L6470_send(0xB0, L6470_SS_PIN2); // モーターを停止
}
