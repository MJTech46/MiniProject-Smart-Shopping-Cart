#include <Wire.h>
#include <U8g2lib.h>
#include <qrcode.h>

// Initialize the display (Ensure your display uses the correct I2C address)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ A5, /* data=*/ A4, /* reset=*/ U8X8_PIN_NONE);

// QR Code data
const char *qrData = "upi://pay?pa=0xa4285@sbi&pn=Team-7&am=999.99";
QRCode qrcode;

void setup() {
  u8g2.begin();
  u8g2.setFlipMode(1); // flip the whole display
}

void loop() {
  displayWelcomeScreen();
  delay(1500);
  
  displayTotalPrice();
  delay(2000);
  
  displayScanAndPay();
  delay(3000);
}

// Function to display the Welcome Screen
void displayWelcomeScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr); // Readable font
  u8g2.setCursor(25, 32);
  u8g2.print("Welcome!");
  u8g2.sendBuffer();
}

// Function to display Total Price Section
void displayTotalPrice() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(25, 12);
  u8g2.print("Total Price");

  u8g2.setFont(u8g2_font_fub20_tr); // font for price
  u8g2.setCursor(5, 55);
  u8g2.print("Rs 999.99");

  u8g2.sendBuffer();
}

// Function to display Scan and Pay with QR Code
void displayScanAndPay() {
  u8g2.clearBuffer();
  
  // Display "Scan to Pay" stacked vertically (one word per line)
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(5, 15);
  u8g2.print("Scan");
  
  u8g2.setCursor(5, 35);
  u8g2.print("to");
  
  u8g2.setCursor(5, 55);
  u8g2.print("Pay");

  // Generate and display the QR Code on the right side
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, qrData);

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        u8g2.drawBox(50 + x * 2, y * 2, 2, 2); // Shift QR to right side
      }
    }
  }
  
  u8g2.sendBuffer();
}
