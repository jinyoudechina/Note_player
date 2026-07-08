//copyright(c) jinyoudechina 2026 all rights reserved.
const int dataPin = 23;
const int clockPin = 18;
const int latchPin = 5;
const int dPin = 26;  //电位器引脚，用于调节音调
const int fPin = 25;  //蜂鸣器引脚

note_t noteList[] = {NOTE_C, NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NOTE_A, NOTE_B};

const byte number_font[] = {
  0xF9,  // 1
  0xA4,  // 2
  0xB0,  // 3
  0x99,  // 4
  0x92,  // 5
  0x82,  // 6
  0xF8   // 7
};

void display1(int n){   //一位数码管显示函数
   shiftOut(dataPin,clockPin,MSBFIRST,number_font[n-1]);//选择要显示的数字，因为数组的索引是从0开始的所以要拿音符-1
   digitalWrite(latchPin,HIGH);//锁住
   digitalWrite(latchPin,LOW);//复位
}

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dPin, INPUT);
  pinMode(fPin, OUTPUT);
  
  analogSetWidth(10);  //设置模拟读取的长度，范围为2^10（0~1023）
  
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, LOW);
  
  ledcSetup(0, 10000, 8);  // 使用通道0，10kHz，8位分辨率
  ledcAttachPin(fPin, 0);  // 通道0
  
  Serial.begin(115200);
  Serial.println("NotePlayer_beta0.0.1");
}

void loop() {
  int Tone = (analogRead(dPin) / 170) + 1;  //将电位器旋转的角度映射为1~7，对应7个音符
  Serial.println(Tone);
  display1(Tone);
  ledcWriteNote(0, noteList[Tone], 4);
  delay(50);
}
