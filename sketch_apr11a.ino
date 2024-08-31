#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Khởi tạo LCD với địa chỉ I2C và kích thước màn hình
LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C có thể khác, kiểm tra trước khi sử dụng

const int buttonStartPin = 2;
const int buttonIncreasePin = 3;
const int buttonDecreasePin = 4;
const int buttonResetPin = 5;
const int relayPin = 13;
const int powerSwitchPin = 6;

long countdownTime = 1800; // Thời gian đếm ngược ban đầu là 30 phút (1800 giây)
bool isCountingDown = false;
unsigned long previousMillis = 0;
const long interval = 1000; // 1 giây

void setup() {
  pinMode(buttonStartPin, INPUT_PULLUP);
  pinMode(buttonIncreasePin, INPUT_PULLUP);
  pinMode(buttonDecreasePin, INPUT_PULLUP);
  pinMode(buttonResetPin, INPUT_PULLUP);
  pinMode(powerSwitchPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  lcd.begin(16, 2); // Truyền tham số số cột và số hàng cho hàm begin
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  updateDisplay();
}

void loop() {
  if (digitalRead(powerSwitchPin) == LOW) {
    delay(200); // Debounce
    digitalWrite(relayPin, LOW); // Tắt relay
    isCountingDown = false;
    countdownTime = 1800; // Reset thời gian về 30 phút
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Power Off");
    while (digitalRead(powerSwitchPin) == LOW) {
      // Chờ cho đến khi công tắc bật lại
      delay(50);
    }
    delay(200); // Debounce
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    updateDisplay();
  }

  unsigned long currentMillis = millis();

  if (digitalRead(buttonStartPin) == LOW) {
    delay(200); // Debounce
    isCountingDown = !isCountingDown;
    if (isCountingDown) {
      digitalWrite(relayPin, HIGH); // Kích hoạt relay
    } 
  }

  if (digitalRead(buttonIncreasePin) == LOW) {
    delay(200); // Debounce
    countdownTime += 1800; // Tăng 30 phút
    updateDisplay();
  }

  if (digitalRead(buttonDecreasePin) == LOW) {
    delay(200); // Debounce
    if (countdownTime >= 1800) countdownTime -= 1800; // Giảm 30 phút
    updateDisplay();
  }

  if (digitalRead(buttonResetPin) == LOW) {
    delay(200); // Debounce
    countdownTime = 0; // Reset thời gian về 0
    isCountingDown = false; // Dừng đếm ngược
    digitalWrite(relayPin, LOW); // Tắt relay
    updateDisplay();
  }

  if (isCountingDown && countdownTime > 0 && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    countdownTime--;
    updateDisplay();
  }

  if (countdownTime == 0 && isCountingDown) {
    digitalWrite(relayPin, LOW); // Tắt relay khi thời gian hết
    isCountingDown = false;
  }
}

void updateDisplay() {
  lcd.setCursor(6, 0);
  lcd.print(countdownTime / 60);
  lcd.print(":");
  int seconds = countdownTime % 60;
  if (seconds < 10) {
    lcd.print("0");
  }
  lcd.print(seconds);
  lcd.print(" ");
}
