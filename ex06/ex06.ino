#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "123";
const char* password = "00000000";

const int LED_PIN_1 = 2;
const int LED_PIN_2 = 5;
const int LED_PIN_3 = 19;

WebServer server(80);
int currentBrightness = 0;

String index_html = R"EOF(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 无极调光</title>
    <style>
        body{font-family: Arial; text-align: center; background: #f4f4f4; padding: 30px;}
        .container{background: white; padding: 40px; border-radius: 15px; display: inline-block;}
        h1{color: #333;}
        .slider{width: 350px; height: 25px; margin: 20px 0;}
        #value{font-size: 28px; font-weight: bold; color: #007bff;}
    </style>
</head>
<body>
    <div class="container">
        <h1>LED 无极调光器</h1>
        <input type="range" min="0" max="255" value="0" class="slider" id="slider" oninput="update()">
        <p>当前亮度：<span id="value">0</span></p>
    </div>
    <script>
        function update() {
            var val = document.getElementById("slider").value;
            document.getElementById("value").innerHTML = val;
            fetch("/set?b=" + val);
        }
    </script>
</body>
</html>
)EOF";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSet() {
  if (server.hasArg("b")) {
    currentBrightness = server.arg("b").toInt();
    currentBrightness = constrain(currentBrightness, 0, 255);
    
    analogWrite(LED_PIN_1, currentBrightness);
    analogWrite(LED_PIN_2, currentBrightness);
    analogWrite(LED_PIN_3, currentBrightness);
    Serial.println(currentBrightness);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  
  analogWrite(LED_PIN_1, 0);
  analogWrite(LED_PIN_2, 0);
  analogWrite(LED_PIN_3, 0);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
}