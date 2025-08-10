#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Reset pin
#define SS_PIN 10  // Slave Select pin

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance
MFRC522::MIFARE_Key key;

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

  // Read name from blocks 8, 9, 10
  Serial.print("Product Name: ");
  readBlocks(8, 3); // Read 3 consecutive blocks starting from block 8

  // Read price from block 12
  Serial.print("Price: ");
  readBlocks(12, 1); // Read 1 block starting from block 12

  // Read quantity from block 13
  Serial.print("Quantity: ");
  readBlocks(13, 1); // Read 1 block starting from block 13

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void readBlocks(byte startBlock, byte blockCount) {
  byte buffer[18];
  byte size = sizeof(buffer);

  for (byte i = 0; i < blockCount; i++) {
    byte block = startBlock + i;

    // Authenticate block
    MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Authentication failed for block ");
      Serial.print(block);
      Serial.print(": ");
      Serial.println(rfid.GetStatusCodeName(status));
      return;
    }

    // Read block
    status = rfid.MIFARE_Read(block, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Read failed for block ");
      Serial.print(block);
      Serial.print(": ");
      Serial.println(rfid.GetStatusCodeName(status));
      return;
    }

    // Print block data
    for (byte j = 0; j < 16; j++) {
      if (buffer[j] != 0x00) { // Ignore padding bytes (0x00)
        Serial.write(buffer[j]);
      }
    }
  }
  Serial.println(); // Newline after reading all blocks
}