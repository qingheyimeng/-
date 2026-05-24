const int touchPin = 4;
const int ledPins[] = {2, 5, 19};
const int ledCount = 3;

bool ledState = false;
bool lastTouchState = false;
const int threshold = 700;

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  pinMode(touchPin, INPUT);
}

void loop() {
  int rawValue = touchRead(touchPin); 
  Serial.println(rawValue);

  bool currentTouchState = (rawValue < threshold);

  if (currentTouchState && !lastTouchState) {
    ledState = !ledState;
    
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledPins[i], ledState);
    }
    
    delay(50);
  }

  lastTouchState = currentTouchState;
  delay(50);
}