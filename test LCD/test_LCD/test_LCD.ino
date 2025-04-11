#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// กำหนดที่อยู่ I2C ของ LCD และขนาด 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init(); // เปลี่ยนจาก lcd.begin(); เป็น lcd.init();
  lcd.backlight(); // เปิดไฟพื้นหลังของ LCD

  lcd.setCursor(0, 0); // กำหนดให้เคอร์เซอร์อยู่ที่ตำแหน่ง 0, แถวที่ 1
  lcd.print("LCD1602 I2C Test"); // แสดงข้อความบรรทัดแรก

  lcd.setCursor(2, 1); // กำหนดให้เคอร์เซอร์อยู่ที่ตำแหน่ง 2, แถวที่ 2
  lcd.print("myarduino.net"); // แสดงข้อความบรรทัดที่สอง
}

void loop() {
  // ไม่มีโค้ดใน loop เพราะต้องการให้ข้อความแสดงตลอดเวลา
}
