#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SOUND_PIN 33
#define RED_LED 18
#define GREEN_LED 19
#define BLUE_LED 23
#define BUZZER_PIN 5

bool alarmNotified = false;

//Filtering: Averages 5 analog readings
int getFilteredValue() {
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(SOUND_PIN);
    delay(2);  // small delay for realistic sampling
  }
  return (sum / 5) / 4;  // average, then scale to 0–1023
}

void setup() {
  Serial.begin(115200);

  // Explicitly set I2C pins for ESP32
  Wire.begin(21, 22);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
    while (true);
  }

  // OLED Test
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 30);
  display.println("READY");
  display.display();
  delay(1000);  // Pause to see the message

  // LED + Buzzer setup
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int val = analogRead(SOUND_PIN);
  val = val / 4; // Convert 0–4095 to 0–1023

  String label = "None";

  // Reset outputs
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  noTone(BUZZER_PIN);

  // Classification
  if (val >= 200 && val <= 400) {
    label = "Door Knock";
    digitalWrite(RED_LED, HIGH);
    alarmNotified = false;
  } else if (val > 400 && val <= 600) {
    label = "Loud door Knock";
    digitalWrite(GREEN_LED, HIGH);
    tone(BUZZER_PIN, 800);
    alarmNotified = false;
  } else if (val > 600) {
    label = "Alarm";
    digitalWrite(BLUE_LED, HIGH);
    tone(BUZZER_PIN, 1000);
    if (!alarmNotified) {
      Serial.println("Sending Notification...");
      alarmNotified = true;
    }
  }

display.clearDisplay();
display.setTextSize(1);
display.setCursor(0, 5); 
display.println("Sound Type:");
display.setTextSize(2);
display.setCursor(0, 25);  
display.println(label);   
display.setTextSize(1);
display.setCursor(0, 55);  
display.print("Level: ");
display.println(val);
display.display();


  delay(200);
}