//copyright(c) jinyoudechina 2026 all rights reserved.
const int dataPin = 23;  //数据引脚
const int clockPin = 18;  //时钟引脚
const int latchPin = 5;  //锁存引脚
const int dPin = 26;  //电位器引脚，用于调节音调
const int fPin = 25;  //蜂鸣器引脚
const int ledPin = 27;  //LED灯引脚
const int touchPin = 4;  //电容触摸传感器引脚

bool should_play = true;  //是否应该播放音符
bool lastTouchState = false;  //上次触摸状态
unsigned long lastTime = 0;  //储存上次按下触摸传感器的时间，用于按键消抖

note_t noteList[] = {NOTE_C, NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NOTE_A, NOTE_B};  //音符列表

const byte number_font[] = {
  0xF9,  // 1
  0xA4,  // 2
  0xB0,  // 3
  0x99,  // 4
  0x92,  // 5
  0x82,  // 6
  0xF8   // 7
};

/**
 * @brief 一位数码管显示函数
 * @param n 要显示的数字 (1-7)
 */
void display1(int n) {
   shiftOut(dataPin, clockPin, MSBFIRST, number_font[n-1]);
   digitalWrite(latchPin, HIGH);
   digitalWrite(latchPin, LOW);
}

/**
 * @brief 日志输出函数
 * @param type 日志类型: true=ERROR, false=INFO
 * @param msg  日志消息（字符串）
 * @param val  可选参数：要输出的数值（默认值 -1 表示不输出）
 * 
 * @example 
 *   print_log(false, "系统启动", -1);           // 输出: [INFO] 系统启动
 *   print_log(false, "触摸值", 45);             // 输出: [INFO] 触摸值: 45
 *   print_log(true, "数组越界", Tone);          // 输出: [ERROR] 数组越界: 5
 *   print_log(false, "播放状态", should_play);  // 输出: [INFO] 播放状态: 1
 * 
 * @note 使用 `-1` 作为 val 的默认值，因为实际输出的数值不可能为 -1
 */
void print_log(bool type, const char* msg, int val = -1) {
  // 根据日志类型选择标签
  if (type == false) {
    Serial.print("[INFO] ");
  } else {
    Serial.print("[ERROR] ");
  }
  
  // 输出消息
  Serial.print(msg);
  
  // 如果 val 不为 -1，则输出数值
  if (val != -1) {
    Serial.print(": ");
    Serial.println(val);
  } else {
    Serial.println();  // 换行
  }
}

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dPin, INPUT);
  pinMode(fPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  analogSetWidth(10);
  
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, LOW);
  
  ledcSetup(0, 10000, 8);
  ledcAttachPin(fPin, 0);
  
  Serial.begin(115200);
  print_log(false, "NotePlayer_beta0.0.2 启动");  // 使用日志函数
  print_log(false, "系统初始化完成");
}

void loop() {
  // 读取当前触摸状态（触摸时返回 true）
  bool touchState = (touchRead(touchPin) < 25);
  
  // 检测下降沿：从未触摸 -> 触摸的瞬间
  if (touchState == true && lastTouchState == false && (millis() - lastTime) > 200) {
    should_play = !should_play;  // 翻转播放状态
    lastTime = millis();
    
    // 使用日志函数输出状态变化
    print_log(false, "触摸触发，播放状态", should_play ? 1 : 0);
  }
  
  // 输出触摸传感器原始值
  int touchValue = touchRead(touchPin);
  print_log(false, "触摸值", touchValue);
  
  // 输出触摸状态
  print_log(false, "触摸状态", touchState ? 1 : 0);
  
  lastTouchState = touchState;  // 更新上次状态
  digitalWrite(ledPin, should_play);  //使用LED灯显示状态
  
  // 读取电位器值
  int Tone = (analogRead(dPin) / 170) + 1;
  
  // 边界检查：防止数组越界
  if (Tone < 1) {
    print_log(true, "数组越界，Tone < 1，已修正为 1", Tone);
    Tone = 1;
  }
  if (Tone > 7) {
    print_log(true, "数组越界，Tone > 7，已修正为 7", Tone);
    Tone = 7;
  }
  
  // 输出当前音符编号
  print_log(false, "当前音符", Tone);
  
  // 显示数字（无论是否播放都显示）
  display1(Tone);
  
  // 根据状态决定是否播放
  if (should_play == true) {
    ledcWriteNote(0, noteList[Tone], 4);
  } else {
    ledcWrite(0, 0);  // 静音
  }
  
  delay(50);
}
