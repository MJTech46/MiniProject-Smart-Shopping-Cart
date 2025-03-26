#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Reset pin
#define SS_PIN 10  // Slave Select pin

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance
MFRC522::MIFARE_Key key;

// Function to split the string into 3 parts
void splitStringToBlocks(String input, byte dataBlocks[3][16]) {
  if (input.length() > 48) {
    String truncated = input.substring(0, 38); // First 38 characters
    String ending = input.substring(input.length() - 5); // Last 5 characters
    input = truncated + "....." + ending; // Add "....." in the middle
  }

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

  // Initialize the key (default key: FFFFFFFFFFFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("*** Welcome To Product Data Writer ***\n");
}

void loop() {
  // Prompt for product details
  String productName, productPrice, productQuantity;

  Serial.println("Enter Product Name:");
  while (!Serial.available()) {} // Wait for input
  productName = Serial.readString();
  productName.trim(); // Remove extra spaces

  Serial.println("Enter Product Price:");
  while (!Serial.available()) {} // Wait for input
  productPrice = Serial.readString();
  productPrice.trim(); // Remove extra spaces

  Serial.println("Enter Product Quantity:");
  while (!Serial.available()) {} // Wait for input
  productQuantity = Serial.readString();
  productQuantity.trim(); // Remove extra spaces

  Serial.println("Now scan the RFID tag...");

  // Wait for RFID tag
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {}

  Serial.println("Card detected!");

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

  // Ready for next input
  Serial.println("All data written successfully!\n\n\n*** Welcome To Product Data Writer ***\n");
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