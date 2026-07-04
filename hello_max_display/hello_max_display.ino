#include <Arduino.h>
#include <Arduino_GFX_Library.h>

constexpr int LCD_DC = 4;
constexpr int LCD_CS = 5;
constexpr int LCD_SCK = 6;
constexpr int LCD_MOSI = 7;
constexpr int LCD_RST = 8;
constexpr int LCD_BL = 15;

constexpr int LCD_WIDTH = 240;
constexpr int LCD_HEIGHT = 280;

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);
Arduino_GFX *gfx = new Arduino_ST7789(
  bus,
  LCD_RST,
  0,
  true,
  LCD_WIDTH,
  LCD_HEIGHT,
  0,
  20,
  0,
  0
);

void drawHelloMax() {
  gfx->fillScreen(RGB565_BLACK);

  gfx->setTextSize(4);
  gfx->setTextColor(RGB565_WHITE);

  const char *message = "hello Max";
  int16_t x = 0;
  int16_t y = 0;
  uint16_t textWidth = 0;
  uint16_t textHeight = 0;
  gfx->getTextBounds(message, 0, 0, &x, &y, &textWidth, &textHeight);

  int16_t cursorX = (LCD_WIDTH - textWidth) / 2;
  int16_t cursorY = (LCD_HEIGHT - textHeight) / 2;
  gfx->setCursor(cursorX, cursorY);
  gfx->println(message);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting hello Max display app");

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed");
    return;
  }

  drawHelloMax();
  Serial.println("Displayed: hello Max");
}

void loop() {
  static unsigned long lastLogMs = 0;

  if (millis() - lastLogMs >= 5000) {
    lastLogMs = millis();
    Serial.println("hello Max display heartbeat");
  }
}
