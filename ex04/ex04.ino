// 定义引脚
const int touchPin = 4;  
const int ledPins[] = {2, 5, 19}; 
const int ledCount = 3; 

// 触摸自锁 + 档位切换变量
bool lastTouchState = false; 
const int threshold = 700;   
int speedGear = 1;           

// 呼吸灯变量
int brightness = 0;    
int fadeStep = 5;    
bool isFadeUp = true; 

void setup() {
  // 串口波特率 115200
  Serial.begin(115200);
  
  // 初始化LED引脚
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    analogWrite(ledPins[i], 0); 
  }
  
  // 初始化触摸引脚
  pinMode(touchPin, INPUT);
  Serial.println("=== 3档触摸调速呼吸灯已启动 ===");
}

void loop() {

  int rawValue = touchRead(touchPin); 
  Serial.print("触摸数值: ");
  Serial.print(rawValue);
  Serial.print("  | 当前档位: ");
  Serial.println(speedGear);

  bool currentTouchState = (rawValue < threshold);

  
  if (currentTouchState == true && lastTouchState == false) {
    speedGear++;          
    if (speedGear > 3) { 
      speedGear = 1;
    }
    delay(50); 
  }
  lastTouchState = currentTouchState;


  int delayTime;  
  switch (speedGear) {
    case 1:
      delayTime = 30;   
      break;
    case 2:
      delayTime = 15;   
    case 3:
      delayTime = 5;    
      break;
    default:
      delayTime = 30;
  }


  if (isFadeUp) {
    brightness += fadeStep;   
    if (brightness >= 255) {  
      brightness = 255;
      isFadeUp = false;      
    }
  } else {
    brightness -= fadeStep;   
    if (brightness <= 0) {    
      brightness = 0;
      isFadeUp = true;      
  }

  // 给所有LED设置亮度
  for (int i = 0; i < ledCount; i++) {
    analogWrite(ledPins[i], brightness);
  }

  delay(delayTime); 
}