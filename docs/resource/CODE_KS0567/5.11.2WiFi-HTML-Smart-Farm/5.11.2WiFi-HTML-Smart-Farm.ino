#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>
#include <dht11.h>
#include <ESP32Servo.h>

// Pin Definitions
#define DHT11PIN        17  // Temperature and humidity sensor pin
#define LEDPIN          27  // LED pin
#define SERVOPIN        26  // Servo pin
#define FANPIN1         19  // Fan IN+ pin
#define FANPIN2         18  // Fan IN- pin
#define STEAMPIN        35  // Steam sensor pin
#define LIGHTPIN        34  // Photoresistor pin
#define SOILHUMIDITYPIN 32  // Soil humidity sensor pin
#define WATERLEVELPIN   33  // Water level sensor pin
#define RELAYPIN        25  // Relay pin

// Initialize sensors and components
dht11 DHT11;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;  // Servo object to control the servo

// WiFi credentials
const char *SSID = "your_SSID";
const char *PASS = "your_PASSWORD";

// Create WebServer object
WebServer server(80);

// Variables for controlling states
static int A = 0;
static int B = 0;
static int C = 0;

// HTML Web page content
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<title>TEST HTML ESP32</title>
<head>
  <meta charset="utf-8">
  <style>
    html, body {
      margin: 0;
      width: 100%;
      height: 100%;
      display: flex;
      justify-content: center;
      align-items: center;
      flex-direction: column;
      background-color: #f0f0f0;
    }

    /* The main button container */
    .btn {
      display: flex;
      justify-content: center;  /* Center the buttons */
      gap: 10px;  /* Add space between buttons */
      width: 320px;  /* Set width to ensure buttons are tightly packed */
      flex-wrap: wrap; /* Allow buttons to wrap to new lines if needed */
      margin-bottom: 20px;  /* Space between buttons and data display */
    }

    /* Button style */
    .btn button {
      width: 70px;  /* Set width for buttons */
      height: 70px;  /* Set height for buttons */
      border: none;
      font-size: 16px;
      color: #fff;
      background-color: #89e689;
      cursor: pointer;
    }

    .btn button:active {
      top: 2px;
    }

    /* The data display area */
    #dht {
      text-align: center;  /* Center the text */
      width: 320px;  /* Same width as the button container */
      color: #fff;
      background-color: #47a047;
      font-size: 18px; /* Adjust font size for readability */
      padding: 10px;
      border-radius: 10px;  /* Rounded corners */
      box-sizing: border-box;
      margin-bottom: 10px; /* Add space between the data display and buttons */
    }

  </style>
</head>
<body>

  <!-- Display area for sensor data -->
  <div id="dht"></div>

  <!-- Button row -->
  <div class="btn">
    <button id="btn-led" onclick="setLED()">LED</button>
    <button id="btn-fan" onclick="setFan()">Fan</button>
    <button id="btn-feeding" onclick="setFeeding()">Feeding</button>
    <button id="btn-watering" onclick="setWatering()">Watering</button>
  </div>

  <script>
    function setLED() {
      var payload = "A"; 
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/set?value=" + payload, true);
      xhr.send();
    }
    function setFan() {
      var payload = "B"; 
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/set?value=" + payload, true);
      xhr.send();
    }
    function setFeeding() {
      var payload = "C"; 
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/set?value=" + payload, true);
      xhr.send();
    }
    function setWatering() {
      var payload = "D"; 
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/set?value=" + payload, true);
      xhr.send();
    }

    setInterval(function () {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("dht").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/dht", true);
      xhttp.send();
    }, 1000)
  </script>

</body>
</html>

)rawliteral";

// Function to merge sensor data into HTML format
String Merge_Data(void) {
  String dataBuffer;
  String Humidity;
  String Temperature;
  String Steam;
  String Light;
  String SoilHumidity;
  String WaterLevel;
  
  // Read DHT11 sensor
  int chk = DHT11.read(DHT11PIN);
  
  // Read other sensors
  Steam = String(analogRead(STEAMPIN) / 4095.0 * 100);
  Light = String(analogRead(LIGHTPIN));
  int shvalue = analogRead(SOILHUMIDITYPIN) / 4095.0 * 100 * 2.3;
  shvalue = shvalue > 100 ? 100 : shvalue;
  SoilHumidity = String(shvalue);
  int wlvalue = analogRead(WATERLEVELPIN) / 4095.0 * 100 * 2.5;
  wlvalue = wlvalue > 100 ? 100 : wlvalue;
  WaterLevel = String(wlvalue);
  Temperature = String(DHT11.temperature);
  Humidity = String(DHT11.humidity);
  
  // Construct HTML content
  dataBuffer += "<p>";
  dataBuffer += "<h1>Sensor Data</h1>";
  dataBuffer += "<b>Temperature:</b><b>" + Temperature + "</b><b>℃</b><br/>";
  dataBuffer += "<b>Humidity:</b><b>" + Humidity + "</b><b>%rh</b><br/>";
  dataBuffer += "<b>WaterLevel:</b><b>" + WaterLevel + "</b><b>%</b><br/>";
  dataBuffer += "<b>Steam:</b><b>" + Steam + "</b><b>%</b><br/>";
  dataBuffer += "<b>Light:</b><b>" + Light + "</b><br/>";
  dataBuffer += "<b>SoilHumidity:</b><b>" + SoilHumidity + "</b><b>%</b><br/>";
  dataBuffer += "</p>";

  return dataBuffer;
}

// Configure actions based on received HTTP requests
void Config_Callback() {
  if (server.hasArg("value")) {
    String HTTP_Payload = server.arg("value");
    Serial.printf("[%lu]%s\r\n", millis(), HTTP_Payload.c_str());

    if (HTTP_Payload == "A") {
      if (A) {
        digitalWrite(LEDPIN, LOW);
        A = 0;
      } else {
        digitalWrite(LEDPIN, HIGH);
        A = 1;
      }
    }

    if (HTTP_Payload == "B") {
      if (B) {
        digitalWrite(FANPIN1, LOW);
        digitalWrite(FANPIN2, LOW);
        B = 0;
      } else {
        delay(500);
        digitalWrite(FANPIN1, HIGH);
        digitalWrite(FANPIN2, LOW);
        delay(500);
        B = 1;
      }
    }

    if (HTTP_Payload == "C") {
      if (C) {
        myservo.write(80);
        delay(500);
        C = 0;
      } else {
        C = 1;
        myservo.write(180);
        delay(500);
      }
    }

    if (HTTP_Payload == "D") {
      digitalWrite(RELAYPIN, HIGH);
      delay(400);
      digitalWrite(RELAYPIN, LOW);
      delay(650);
    }
  }
  server.send(200, "text/plain", "OK");
}

// Handle invalid URL access
void notFound() {
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(9600);
  
  // Connect to WiFi
  WiFi.begin(SSID, PASS);
  while (!WiFi.isConnected()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up pins
  pinMode(LEDPIN, OUTPUT);
  pinMode(STEAMPIN, INPUT);
  pinMode(LIGHTPIN, INPUT);
  pinMode(SOILHUMIDITYPIN, INPUT);
  pinMode(WATERLEVELPIN, INPUT);
  pinMode(RELAYPIN, OUTPUT);
  pinMode(FANPIN1, OUTPUT);
  pinMode(FANPIN2, OUTPUT);

  delay(1000);

  // Attach servo to the pin
  myservo.attach(SERVOPIN);
  myservo.write(180);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  // Set up server routes
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", index_html);
  });

  server.on("/dht", HTTP_GET, []() {
    server.send(200, "text/plain", Merge_Data().c_str());
  });

  server.on("/set", HTTP_GET, Config_Callback);
  server.onNotFound(notFound);

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();
}
