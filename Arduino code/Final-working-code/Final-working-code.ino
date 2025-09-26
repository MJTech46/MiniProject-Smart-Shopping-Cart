#include <SPI.h>
#include <MFRC522.h>
#include <U8g2lib.h>

#define RST_PIN 9
#define SS_PIN 10

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

const byte MAX_ITEMS = 5;  // Increased capacity since we're not storing names
struct InventoryItem {
  byte uid[4];   // 4 bytes for UID
  float price;   // 4 bytes for price
};
InventoryItem inventory[MAX_ITEMS];
byte itemCount = 0;
float totalValue = 0.0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  showDisplayMessage("System Ready");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  byte uid[4];
  memcpy(uid, rfid.uid.uidByte, 4);
  byte itemIndex = findItemIndex(uid);

  // Read product name from tag (but don't store it)
  char productName[12];
  bool hasName = readProductName(productName);
  float price = readProductPrice();

  if (itemIndex == 0xFF && itemCount < MAX_ITEMS && price > 0) {
    addItem(uid, price);
    showLiveTagInfo("ADDED", productName, price, hasName);
  } 
  else if (itemIndex != 0xFF) {
    float removedPrice = inventory[itemIndex].price;
    removeItem(itemIndex);
    showLiveTagInfo("REMOVED", productName, removedPrice, hasName);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);
}

// Display live tag info (name is read fresh each time)
void showLiveTagInfo(const char* action, const char* productName, float price, bool hasName) {
  u8g2.clearBuffer();
  
  // Action line
  u8g2.drawStr(0, 10, action);
  
  // Product name (if available)
  if (hasName) {
    u8g2.drawStr(40, 10, ":");
    u8g2.drawStr(45, 10, productName);
  }
  
  // Price line
  char priceStr[16];
  dtostrf(price, 5, 2, priceStr);
  u8g2.drawStr(0, 25, "Price: $");
  u8g2.drawStr(50, 25, priceStr);
  
  // Total line
  char totalStr[16];
  dtostrf(totalValue, 5, 2, totalStr);
  u8g2.drawStr(0, 40, "Total: $");
  u8g2.drawStr(50, 40, totalStr);
  
  u8g2.sendBuffer();

  // Serial output
  // Serial.print(action);
  // if (hasName) {
  //   Serial.print(" ");
  //   Serial.print(productName);
  // }
  // Serial.print(" - Price: $");
  // Serial.print(price, 2);
  // Serial.print(" - Total: $");
  // Serial.println(totalValue, 2);
}

// Inventory management (only UID and price)
void addItem(byte uid[4], float price) {
  memcpy(inventory[itemCount].uid, uid, 4);
  inventory[itemCount].price = price;
  itemCount++;
  totalValue += price;
}

void removeItem(byte index) {
  totalValue -= inventory[index].price;
  for (byte i = index; i < itemCount - 1; i++) {
    memcpy(inventory[i].uid, inventory[i+1].uid, 4);
    inventory[i].price = inventory[i+1].price;
  }
  itemCount--;
}

// Helper functions
byte findItemIndex(byte uid[4]) {
  for (byte i = 0; i < itemCount; i++) {
    if (memcmp(uid, inventory[i].uid, 4) == 0) {
      return i;
    }
  }
  return 0xFF;
}

bool readProductName(char* buffer) {
  byte sectorBuffer[18];
  byte size = sizeof(sectorBuffer);
  byte pos = 0;
  memset(buffer, 0, 12);

  for (byte block = 8; block <= 10 && pos < 11; block++) {
    byte sector = block / 4 * 4 + 3;
    if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector, &key, &(rfid.uid)) == MFRC522::STATUS_OK) {
      if (rfid.MIFARE_Read(block, sectorBuffer, &size) == MFRC522::STATUS_OK) {
        for (byte i = 0; i < 16 && pos < 11; i++) {
          if (sectorBuffer[i] > 31 && sectorBuffer[i] < 127) {
            buffer[pos++] = (char)sectorBuffer[i];
          }
        }
      }
    }
  }
  return (pos > 0); // Returns true if name was read
}

float readProductPrice() {
  byte buffer[18];
  byte size = sizeof(buffer);
  char priceStr[8];
  byte pos = 0;

  if (authenticate(12)) {
    if (rfid.MIFARE_Read(12, buffer, &size) == MFRC522::STATUS_OK) {
      for (byte i = 0; i < 16 && pos < 7; i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9' || buffer[i] == '.') {
          priceStr[pos++] = (char)buffer[i];
        }
      }
      priceStr[pos] = '\0';
      return atof(priceStr);
    }
  }
  return 0.0;
}

bool authenticate(byte block) {
  byte sector = block / 4 * 4 + 3;
  return rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector, &key, &(rfid.uid)) == MFRC522::STATUS_OK;
}

void showDisplayMessage(const char* message) {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, message);
  u8g2.sendBuffer();
}
