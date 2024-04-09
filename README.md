# Advanced Cart System [VARS CART]

![System Snapshot](images/system_snapshot.jpg)

This Arduino project implements an RFID-based smart shopping cart system. It utilizes an Arduino board, an RFID reader module, a servo motor, a liquid crystal display (LCD), and other components to create a system where users can add or remove items to/from their cart by scanning RFID tags. The system calculates the total price of the items added and displays it. It also has functionality to remove items from the cart and finalize the transaction using a master RFID card.

## Table of Contents
- [Components Required](#components-required)
- [Setup Instructions](#setup-instructions)
- [How It Works](#how-it-works)
- [Usage](#usage)
- [Author](#author)
- [License](#license)

## Components Required
- Arduino board
- MFRC522 RFID reader module
- Liquid crystal display (LCD) with I2C interface
- Servo motor
- Buzzer
- Tactile switch
- LEDs (optional)

## Setup Instructions
1. Connect the components as per the circuit diagram provided.
2. Install the necessary libraries in the Arduino IDE: `Wire`, `LiquidCrystal_I2C`, `MFRC522`, and `Servo`.
3. Upload the provided Arduino sketch to your Arduino board.
4. Power up the system and place RFID tags near the RFID reader to interact with the system.

## How It Works
The system operates as follows:
- Upon startup, it initializes the components and displays a welcome message on the LCD.
- It continuously checks for RFID tags placed near the RFID reader.
- Users can add items to their cart by scanning the RFID tags associated with the items.
- The LCD displays the name and price of the added item and updates the total price.
- Users can remove items from the cart by pressing a tactile switch.
- When a master RFID card is detected, the transaction is finalized, and the total price is displayed.
- After the transaction is complete, additional scanning of RFID tags results in a "Not Applicable" message.

## Usage
1. Power up the system and wait for the initialization message on the LCD.
2. Scan RFID tags of items to add them to the cart.
3. Optionally, remove items by pressing the tactile switch.
4. Use the master RFID card to finalize the transaction and view the total price.
5. After the transaction is complete, further scanning of RFID tags will display a "Not Applicable" message.

## Author
This project was created by [Viplove Parsai].

## License
This project is licensed under the [MIT License](LICENSE).
