//auth
#define BLYNK_TEMPLATE_ID "TMPL2VDCRCY49"
#define BLYNK_TEMPLATE_NAME "temp and hum monitor"
#define BLYNK_AUTH_TOKEN ""
//blynk imports
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiClient.h>
BlynkTimer timer;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "";
char pass[] = "";

//Temp. Humidity sensor
#include <DHT.h>

//OLED display
#include <U8g2lib.h>

//common library
#include <Wire.h>

//PINOUTS

//DHT-11
#define DHTPIN 19 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

//OLED
#define OLED_SDA 21       
#define OLED_SCL 22
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA);  // Initialize U8g2 display object

//current
const int sensorIn = 35;

//ultrasonic sensor
#define echoPin 26               
#define trigPin 27              
long duration;
double distance;


void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  dht.begin();
  u8g2.begin();

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  timer.run();

  delay(2000);  // Wait for 2 seconds
 
  // Read temperature and humidity values from DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  //ACS712
  float ACSValue = 0.0, Samples = 0.0, AvgACS = 0.0, BaseVol = 1.53; //Change BaseVol as per your reading in the first step.
  for (int x = 0; x < 500; x++) { //This would take 500 Samples
    ACSValue = analogRead(sensorIn);
    Samples = Samples + ACSValue;
    delay (3);
  }
  AvgACS = Samples/500;
  Serial.println((((AvgACS) * (3.3 / 4095.0)) - BaseVol ) / 0.066 );
  float curr = abs((((AvgACS) * (3.3 / 4095.0)) - BaseVol ) / 0.066);
  //----------

  //HC-SR-04
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  distance = (((5.5-distance)/(5.5)))*100;
  String oil = String(distance);
  //----------

  // Print temperature and humidity values to Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Send data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, curr);
  Blynk.virtualWrite(V3, distance);

  // Display temperature, humidity, current, oil level on OLED display
  u8g2.clearBuffer();  // Clear the display buffer
  u8g2.setFont(u8g2_font_6x10_tf);  // Set the font size
  u8g2.setCursor(0, 10);  // Set the cursor position
  u8g2.print("Humidity: ");
  u8g2.print(humidity);
  u8g2.print("%");
  u8g2.setCursor(0, 20);
  u8g2.print("Temperature: ");
  u8g2.print(temperature);
  u8g2.print("C");

  u8g2.setCursor(0, 30);
  u8g2.print("Current: ");
  u8g2.print(curr);
  u8g2.print("A");

  u8g2.setCursor(0, 40);
  u8g2.print("Oil level: ");
  u8g2.print(distance);
  u8g2.print("%");

  u8g2.sendBuffer();  // Send the buffer to the display
}
