#include <Wire.h>
#include <U8g2lib.h>
#include <qrcode.h>

// Initialize U8g2 for SSH1106
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  // Start OLED display
  u8g2.begin();
  u8g2.clearBuffer();

  // Generate QR Code
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)]; // Version 3 QR code
  qrcode_initText(&qrcode, qrcodeData, 3, 0, "upi://pay?pa=0xa4285@sbi&pn=Team-7&am=1111.11");

  // Draw QR Code
  drawQRCode(qrcode);

  // Send buffer to display
  u8g2.sendBuffer();
}

void loop() {
  // No need for repeated code
}

void drawQRCode(QRCode &qrcode) {
  int scale = 2; // Scale factor for better visibility
  int xOffset = (128 - qrcode.size * scale) / 2; // Center horizontally
  int yOffset = (64 - qrcode.size * scale) / 2;  // Center vertically
  
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        u8g2.drawBox(xOffset + x * scale, yOffset + y * scale, scale, scale);
      }
    }
  }
}
