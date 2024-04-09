#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h> // Include the MFRC522 library
#include <Servo.h>   // Include the Servo library

#define SS_PIN 10
#define RST_PIN 9
#define SWITCH_PIN 8 // Tactile switch pin
#define BUZZER_PIN 4 // Buzzer pin
#define SERVO_PIN 3  // Servo motor pin
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;  // create servo object to control a servo

char input[12];
int count = 0;

double total = 0.0; // Initialize total price as 0.0
int count_prod = 0;
bool masterCardDetected = false; // Flag to track master card detection
bool transactionComplete = false; // Flag to track if transaction is complete
bool removeItem = false; // Flag to track if item removal is requested

// Define the UIDs, names, and prices for the items
String itemUIDs[9] = {"D04A0F25", "136F41F6", "D0C70C25", "D0619525", "D0703725", "D0002625", "D0866825", "D0632525"};
String itemNames[9] = {"Bsmt Rice", "Biscuit", "Colgate", "Oats 1kg", "Sugar 1kg", "Coffee", "Almonds", "Namkeen", "Item 9"};
double itemPrices[9] = {106.00, 30.00, 80.00, 400.00, 38.00, 154.00, 863.56, 65.00, 900.00};

void setup() {
  pinMode(A4, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP); // Set switch pin as input
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output

  lcd.init();
  lcd.backlight();
  lcd.clear();
  Wire.begin();
  SPI.begin(); // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522
  Serial.begin(9600);
  lcd.setCursor(3, 0);
  lcd.print("VARS CART");
  delay(3000);
  lcd.setCursor(2, 0);
  lcd.clear();
  lcd.print("Let's Shop !!");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Add Item !!");

  myservo.attach(SERVO_PIN);  // attaches the servo on pin 3 to the servo object
  myservo.write(0); // Initial position
}

void loop() {
  count = 0;
  while (Serial.available() && count < 12) {
    input[count] = Serial.read();
    Serial.print(input[count]);
    Serial.print(" ");
    count++;
    delay(5);
  }

  // Check for switch press to remove item
  if (digitalRead(SWITCH_PIN) == LOW) {
    removeItem = true;
  }

  // Check for RFID card
  if (!transactionComplete && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase();

    Serial.print("Card UID: ");
    Serial.println(cardUID);

    // Process product addition or removal
    for (int i = 0; i < 9; i++) {
      if (cardUID == itemUIDs[i]) {
        if (!removeItem) {
          // Add product and update total
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(itemNames[i] + " Added");
          lcd.setCursor(0, 1);
          lcd.print("Price: " + String(itemPrices[i]));
          total += itemPrices[i];
          count_prod++;
          digitalWrite(6, HIGH); // Green LED (assuming)
          digitalWrite(7, LOW);  // Turn off red LED (assuming)
          digitalWrite(BUZZER_PIN,HIGH);
          delay(1000);
          digitalWrite(BUZZER_PIN,LOW);
          digitalWrite(6,LOW); // Green LED (assuming)
          // tone(BUZZER_PIN, 1000, 200); // Sound the buzzer
          // myservo.write(180); // Rotate servo to 180 degrees
          // delay(5000); // Wait for 5 seconds
          // myservo.write(0); // Rotate servo back to 0 degrees
          lcd.clear();
        } else if (count_prod > 0 && total >= itemPrices[i]) {
          // Remove product and update total
          count_prod--;
          total -= itemPrices[i];
          lcd.clear();
          lcd.setCursor(0, 0);
          digitalWrite(6, HIGH); // Turn off green LED (assuming)
          digitalWrite(BUZZER_PIN,HIGH);
          lcd.print(itemNames[i] + " Removed");
          lcd.setCursor(0, 1);
          lcd.print("Price: " + String(itemPrices[i]));
          delay(1000);
          digitalWrite(BUZZER_PIN,LOW);
          digitalWrite(6, LOW); // Turn off green LED (assuming)
          delay(1000);
          lcd.clear();
          removeItem = false; // Reset the flag
          // digitalWrite(7, HIGH);  // Red LED (assuming)
          // tone(BUZZER_PIN, 1000, 200); // Sound the buzzer
          // myservo.write(180); // Rotate servo to 180 degrees
          // delay(5000); // Wait for 5 seconds
          // myservo.write(0); // Rotate servo back to 0 degrees
        }
      }
    }

    // Process master card
    if (cardUID == "B3B454F4") { // Replace with your master card ID
      masterCardDetected = true;
      transactionComplete = true;
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

  // Display add another item message after product addition
  if (!masterCardDetected && count_prod > 0) {
    lcd.setCursor(0, 0);
    lcd.print("Add Another Item !");
  }

  // Display total on master card detection
  if (masterCardDetected) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Items Qty:");
    lcd.setCursor(10, 0);
    lcd.print(count_prod);
    lcd.setCursor(0, 1);
    lcd.print("Total :");
    lcd.setCursor(8, 1);
    lcd.print(total);
    delay(1000); // Display total for 5 seconds
  }

  // Display "Not Applicable" if a card is scanned after transaction is complete
  if (transactionComplete && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Not Applicable");
    delay(2000); // Display message for 2 seconds
    lcd.clear();
  }
}