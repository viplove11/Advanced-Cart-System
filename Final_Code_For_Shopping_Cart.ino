// including all necessary Header files
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>  // Include the MFRC522 library

// defining all necessary pins
#define SS_PIN 10
#define RST_PIN 9
#define SWITCH_PIN 8               // Tactile switch pin
#define BUZZER_PIN 4               // Buzzer pin
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// creating the object of LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

char input[12];
int count = 0;

// defining  the indicator for displaying bill
int indicator = 0;

// defining variable =
double total = 0.0;  // Initialize total price as 0.0
int count_prod = 0;
bool masterCardDetected = false;   // Flag to track master card detection
bool transactionComplete = false;  // Flag to track if transaction is complete
bool removeItem = false;           // Flag to track if item removal is requested

// Define the UIDs, names, and prices for the items
String itemUIDs[9] = { "D04A0F25", "136F41F6", "D0C70C25", "D0619525", "D0703725", "D0002625", "D0866825", "D0632525" };
String itemNames[9] = { "Bsmt Rice", "Biscuit", "Colgate", "Oats 1kg", "Sugar 1kg", "Coffee", "Almonds", "Namkeen" };
double itemPrices[9] = { 106.00, 30.00, 80.00, 400.00, 38.00, 154.00, 863.56, 65.00 };
int itemCounts[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // Initialize item counts as 0

// setup() functions
void setup() {
  pinMode(A4, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Set switch pin as input
  pinMode(BUZZER_PIN, OUTPUT);        // Set buzzer pin as output

  lcd.init();
  lcd.backlight();
  lcd.clear();
  Wire.begin();
  SPI.begin();         // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize MFRC522
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
}

// loop functions()
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
          digitalWrite(6, HIGH);  // Green LED (assuming)
          digitalWrite(BUZZER_PIN, HIGH); // writing buzzer to high
          total += itemPrices[i];
          count_prod++;
          itemCounts[i]++;  // Increase item count
          delay(500);
          digitalWrite(BUZZER_PIN, LOW);
          digitalWrite(6, LOW);  // Green LED (assuming)
          lcd.clear();
        } else if (count_prod > 0 && total >= itemPrices[i] && itemCounts[i] > 0) {
          // Remove product and update total
          count_prod--;
          total -= itemPrices[i];
          itemCounts[i]--;  // Decrease item count
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(itemNames[i] + " Removed");
          lcd.setCursor(0, 1);
          lcd.print("Price: " + String(itemPrices[i]));
          digitalWrite(6, HIGH);           // Turn on green LED (assuming)
          digitalWrite(BUZZER_PIN, HIGH);  
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(itemNames[i] + " Count:");
          lcd.setCursor(0, 1);
          lcd.print(itemCounts[i]);
          delay(1200);                    // delay for 1.2 seconds
          digitalWrite(6, LOW);           // Turn off green LED 
          digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer
          lcd.clear();
          removeItem = false;  // Reset the flag
        }
      }
    }

    // Process master card
    if (cardUID == "B3B454F4") {  // Replace with your master card ID
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
    // Display all items with their counts
    if (indicator == 0) { // display the list of all item , when indicator is at value 0
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Displaying Bill!");
      delay(2000);
      lcd.clear();
      for (int i = 0; i < 9; i++) {
        if (itemCounts[i] > 0) {  // Only display items with count > 0
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Getting Count !!");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(itemNames[i] + " :");
          lcd.setCursor(0, 1);
          lcd.print(itemCounts[i]);
          delay(2000);  // Display each item count for 2 seconds
        }
      }
      indicator = 1;
    } else { // when indicator is 1 , only display the bill with its total quantity
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Items Qty:");
      lcd.setCursor(10, 0);
      lcd.print(count_prod);
      lcd.setCursor(0, 1);
      lcd.print("Total :");
      lcd.setCursor(8, 1);
      lcd.print(total);
      delay(5000);  // Display total for 5 seconds
    }
  }

  // Display "Not Applicable" if a card is scanned after transaction is complete
  if (transactionComplete && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Not Applicable");
    delay(2000);  // Display message for 2 seconds
    lcd.clear();
  }
}
