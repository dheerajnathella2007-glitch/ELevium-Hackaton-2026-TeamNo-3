#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
#define DHTPIN 4            // DHT11 data pin
#define DHTTYPE DHT11     
#define BUTTON_PIN 18       // Push button simulating vibration

// Temperature LEDs (3 LEDs)
#define TEMP_GREEN 13       // Normal temp (< 35°C)
#define TEMP_YELLOW 12      // Warning temp (35°C - 45°C)
#define TEMP_RED 14         // Critical temp (>= 45°C)

// Vibration LEDs (2 LEDs)
#define VIB_GREEN 27        // Normal vibration (Button not pressed)
#define VIB_RED 26          // Critical vibration (Button pressed)

// Thresholds
const float TEMP_WARNING = 35.0;
const float TEMP_CRITICAL = 45.0;

// Initialize Sensors and Display
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(115200);
  
  // Initialize Pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(TEMP_GREEN, OUTPUT);
  pinMode(TEMP_YELLOW, OUTPUT);
  pinMode(TEMP_RED, OUTPUT);
  
  pinMode(VIB_GREEN, OUTPUT);
  pinMode(VIB_RED, OUTPUT);

  // Initialize DHT and LCD
  dht.begin();
  lcd.init();                      
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Radar Health Mon");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  // 1. Read DHT11 Temperature & Humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    temperature = 0.0;
    humidity = 0.0;
  }

  // 2. Read Push Button State (LOW = Pressed/Critical Vibration, HIGH = Normal)
  bool isButtonPressed = (digitalRead(BUTTON_PIN) == LOW);

  // 3. Control Temperature LEDs (Green, Yellow, Red)
  if (temperature >= TEMP_CRITICAL) {
    digitalWrite(TEMP_RED, HIGH);
    digitalWrite(TEMP_YELLOW, LOW);
    digitalWrite(TEMP_GREEN, LOW);
  } else if (temperature >= TEMP_WARNING) {
    digitalWrite(TEMP_RED, LOW);
    digitalWrite(TEMP_YELLOW, HIGH);
    digitalWrite(TEMP_GREEN, LOW);
  } else {
    digitalWrite(TEMP_RED, LOW);
    digitalWrite(TEMP_YELLOW, LOW);
    digitalWrite(TEMP_GREEN, HIGH);
  }

  // 4. Control Vibration LEDs (Separate Green, Red)
  if (isButtonPressed) {
    digitalWrite(VIB_RED, HIGH);   // Critical vibration indicator ON
    digitalWrite(VIB_GREEN, LOW);  // Normal vibration indicator OFF
  } else {
    digitalWrite(VIB_RED, LOW);    // Critical vibration indicator OFF
    digitalWrite(VIB_GREEN, HIGH); // Normal vibration indicator ON
  }

  // 5. Display Status on LCD1602 (16x2 Layout)
  // Line 1: Temperature & Humidity
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print((int)humidity);
  lcd.print("% ");

  // Line 2: Separate status for Temperature and Vibration
  lcd.setCursor(0, 1);
  // T_Status: N=Normal, W=Warning, C=Critical
  // V_Status: N=Normal, C=Critical (Pressed)
  char tempCode = (temperature >= TEMP_CRITICAL) ? 'C' : (temperature >= TEMP_WARNING) ? 'W' : 'N';
  
  lcd.print("T:");
  lcd.print(tempCode);
  lcd.print("  V:");
  if (isButtonPressed) {
    lcd.print("CRIT (Press)");
  } else {
    lcd.print("NORM (Stable)");
  }

  // Serial Monitor Debugging
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print("C | Vib Button: "); Serial.println(isButtonPressed ? "PRESSED (Vib Red ON)" : "NOT PRESSED (Vib Green ON)");

  delay(500);
}