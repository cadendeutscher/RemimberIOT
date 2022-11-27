
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <string>

#define SDA 13
#define SCL 14

const char* ssid = "TMOBILE-D589";
const char* password = "waterytree684";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://cadendeutscher.pythonanywhere.com/today";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

int content = 0;

LiquidCrystal_I2C lcd_i2c(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

// CREDIT TO https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/ for the following method.
// Function to scroll text
// The function acepts the following arguments:
// row: row number where the text will be displayed
// message: message to scroll
// delayTime: delay between each character shifting
// lcdColumns: number of columns of your LCD
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd_i2c.setCursor(0, row);
    lcd_i2c.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

void setup() {
    Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Conecting to WIFI...");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

  Wire.begin(SDA,SCL);
  lcd_i2c.init();
  lcd_i2c.backlight();

}

void loop() {
 
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      HTTPClient http;
     
      // Your Domain name with URL path or IP address with path
      http.useHTTP10(true);
      http.begin(serverName);
     
      // Send HTTP GET request
      int httpResponseCode = http.GET();
     
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        JsonArray info = doc.as<JsonArray>();
        // Read values
        int reminder = 1;
        for(JsonVariant v : info) {
           Serial.println(v["content"].as<String>());
           String s = v["content"].as<String>();
           lcd_i2c.clear();              
           lcd_i2c.setCursor(0, 0); 
               String num = String(reminder);
         
               lcd_i2c.print("Reminder " + num +" of " + info.size());
               scrollText(1,s,500,16);
           reminder++;
           
        }
        
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
 
  }
     else {
      Serial.println("WiFi Disconnected");
    }
  delay(10000);

}
