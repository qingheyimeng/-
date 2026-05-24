#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "123";
const char* password = "00000000";

const int LED_PIN_1 = 2;
const int LED_PIN_2 = 5;
const int LED_PIN_3 = 19;
const int touchPin = 4;
const int threshold = 700;

bool isArmed = false;
bool isAlarming = false;
int currentTouchValue = 0;

WebServer server(80);

String index_html = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 实时传感器仪表盘</title>
<style>
  body{text-align:center; font-family:Arial; margin-top:30px; background:#f5f5f5;}
  .panel{background:white; padding:40px; border-radius:20px; width:320px; display:inline-block; box-shadow:0 0 15px rgba(0,0,0,0.1);}
  .value{font-size:60px; font-weight:bold; color:#007bff; margin:40px 0;}
  .label{font-size:20px; color:#666;}
  .btn{width:140px; height:50px; font-size:18px; margin:10px; border:none; border-radius:8px; color:white;}
  .arm{background:#ff3333;}
  .disarm{background:#33cc33;}
</style>
</head>
<body>
<div class="panel">
  <h2>触摸传感器实时值</h2>
  <div class="label">当前触摸数值：</div>
  <div class="value" id="touchValue">0</div>
  <h4>安防控制</h4>
  <button class="btn arm" onclick="arm()">布防</button>
  <button class="btn disarm" onclick="disarm()">撤防</button>
</div>
<script>
function getTouchValue() {
  fetch("/sensor").then(response => response.text()).then(data => {
    document.getElementById("touchValue").innerHTML = data;
  });
}
setInterval(getTouchValue, 100);
function arm(){ fetch("/arm"); }
function disarm(){ fetch("/disarm"); }
</script>
</body>
</html>
)HTML";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSensor() {
  server.send(200, "text/plain", String(currentTouchValue));
}

void handleArm() {
  isArmed = true;
  isAlarming = false;
  server.send(200, "text/plain", "Armed");
}

void handleDisarm() {
  isArmed = false;
  isAlarming = false;
  analogWrite(LED_PIN_1, 0);
  analogWrite(LED_PIN_2, 0);
  analogWrite(LED_PIN_3, 0);
  server.send(200, "text/plain", "Disarmed");
}

void setAllLED(int value) {
  analogWrite(LED_PIN_1, value);
  analogWrite(LED_PIN_2, value);
  analogWrite(LED_PIN_3, value);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(touchPin, INPUT);
  
  setAllLED(0);

  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("连接 WiFi: ");
  Serial.println(ssid);
  int timeout = 20;
  while (WiFi.status() != WL_CONNECTED && timeout-- > 0) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi 连接成功！");
    Serial.print("仪表盘地址: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi 连接失败！");
  }

  server.on("/", handleRoot);
  server.on("/sensor", handleSensor);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  
  server.begin();
}

void loop() {
  server.handleClient();
  currentTouchValue = touchRead(touchPin);

  if (isArmed && !isAlarming) {
    if (currentTouchValue < threshold) {
      isAlarming = true;
    }
  }

  if (isAlarming) {
    setAllLED(255);
    delay(100);
    setAllLED(0);
    delay(100);
  }

  delay(50);
}