#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Reset pin
#define SS_PIN 10  // Slave Select pin

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance
MFRC522::MIFARE_Key key;

// Function to split the string into 3 parts
void splitStringToBlocks(String input, byte dataBlocks[3][16]) {
  // Ensure the string is no longer than 48 characters
  if (input.length() > 48) {
    String truncated = input.substring(0, 38); // First 38 characters
    String ending = input.substring(input.length() - 5); // Last 5 characters
    input = truncated + "....." + ending; // Add "....." in the middle
}

  // Split the string into 3 blocks of 16 bytes each
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 16; j++) {
      int index = i * 16 + j;
      if (index < input.length()) {
        dataBlocks[i][j] = input[index];
      } else {
        dataBlocks[i][j] = 0x00; // Fill remaining space with 0x00
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();         // Initialize SPI bus
  rfid.PCD_Init();     // Initialize MFRC522
  Serial.println("Scan an RFID tag...");
  
  // Initialize the key (default key: FFFFFFFFFFFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  // Look for a card
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("\n\nCard detected!");

  // Product details
  String productName = "Colgate Strong Teeth, 150G, India S No: 1 Toothpaste Brand, Calcium-Boost Tooth Paste For 2X Stronger Teeth, Whitens Teeth, Fresh Breath & Prevents Cavities"; // Example long name
  String productPrice = "49.00"; // Example price as string
  String productQuantity = "1"; // Example quantity as string

  // Split product name into 3 blocks
  byte nameBlocks[3][16];
  splitStringToBlocks(productName, nameBlocks);

  // Write name data to blocks 8, 9, and 10
  for (int i = 0; i < 3; i++) {
    if (writeBlock(8 + i, nameBlocks[i])) {
      Serial.print("Block ");
      Serial.print(8 + i);
      Serial.println(" written successfully!");
    } else {
      Serial.print("Failed to write to block ");
      Serial.println(8 + i);
    }
  }

  // Write price to block 12
  byte priceBlock[16] = {0};
  productPrice.getBytes(priceBlock, 16); // Convert price string to bytes
  if (writeBlock(12, priceBlock)) {
    Serial.println("Price written successfully!");
  } else {
    Serial.println("Failed to write price.");
  }

  // Write quantity to block 13
  byte quantityBlock[16] = {0};
  productQuantity.getBytes(quantityBlock, 16); // Convert quantity string to bytes
  if (writeBlock(13, quantityBlock)) {
    Serial.println("Quantity written successfully!");
  } else {
    Serial.println("Failed to write quantity.");
  }

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

bool writeBlock(byte block, byte *dataBlock) {
  MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for block ");
    Serial.print(block);
    Serial.print(": ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  status = rfid.MIFARE_Write(block, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed for block ");
    Serial.print(block);
    Serial.print(": ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }
  return true;
}