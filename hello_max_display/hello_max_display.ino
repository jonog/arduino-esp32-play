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
constexpr unsigned long COLOR_CHANGE_INTERVAL_MS = 2000;
constexpr const char *MESSAGE = "hello Max";

const uint16_t TEXT_COLORS[] = {
  RGB565_WHITE,
  RGB565_RED,
  RGB565_GREEN,
  RGB565_BLUE,
  RGB565_CYAN,
  RGB565_MAGENTA,
  RGB565_YELLOW,
  RGB565_ORANGE
};

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

void drawHelloMax(uint16_t color) {
  gfx->fillScreen(RGB565_BLACK);

  gfx->setTextSize(4);
  gfx->setTextColor(color);

  int16_t x = 0;
  int16_t y = 0;
  uint16_t textWidth = 0;
  uint16_t textHeight = 0;
  gfx->getTextBounds(MESSAGE, 0, 0, &x, &y, &textWidth, &textHeight);

  int16_t cursorX = (LCD_WIDTH - textWidth) / 2;
  int16_t cursorY = (LCD_HEIGHT - textHeight) / 2;
  gfx->setCursor(cursorX, cursorY);
  gfx->println(MESSAGE);
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

  drawHelloMax(TEXT_COLORS[0]);
  Serial.println("Displayed: hello Max");
}

void loop() {
  static unsigned long lastLogMs = 0;
  static unsigned long lastColorChangeMs = 0;
  static size_t colorIndex = 0;

  if (millis() - lastColorChangeMs >= COLOR_CHANGE_INTERVAL_MS) {
    lastColorChangeMs = millis();
    colorIndex = (colorIndex + 1) % (sizeof(TEXT_COLORS) / sizeof(TEXT_COLORS[0]));
    drawHelloMax(TEXT_COLORS[colorIndex]);
  }

  if (millis() - lastLogMs >= 5000) {
    lastLogMs = millis();
    Serial.println("hello Max display heartbeat");
  }
}
