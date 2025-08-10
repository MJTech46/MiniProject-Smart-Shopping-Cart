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

  Serial.println("Card detected!");

  // Write Data to Block 4
  byte block = 8;
  //byte dataBlock[16] = "abcdefghijklmnop"; // Data to write (16 bytes)
  byte dataBlock[16] = "1234567890123456"; // Data to write (16 bytes)
  
  if (writeBlock(block, dataBlock)) {
    Serial.println("Data written successfully!");
  } else {
    Serial.println("Failed to write data.");
  }

  // Read Data from Block 4
  byte buffer[18];
  byte size = sizeof(buffer);

  if (readBlock(block, buffer, size)) {
    Serial.print("Data read from block: ");
    for (byte i = 0; i < 16; i++) {
      Serial.write(buffer[i]);
    }
    Serial.println();
  } else {
    Serial.println("Failed to read data.");
  }

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

bool writeBlock(byte block, byte *dataBlock) {
  MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed.");
    return false;
  }

  status = rfid.MIFARE_Write(block, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Write failed.");
    return false;
  }
  return true;
}

bool readBlock(byte block, byte *buffer, byte &bufferSize) {
  MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed.");
    return false;
  }

  status = rfid.MIFARE_Read(block, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Read failed.");
    return false;
  }
  return true;
}
