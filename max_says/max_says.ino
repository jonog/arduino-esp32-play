#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <CST816S.h>

#include "max_says_game.h"

constexpr int LCD_DC = 4;
constexpr int LCD_CS = 5;
constexpr int LCD_SCK = 6;
constexpr int LCD_MOSI = 7;
constexpr int LCD_RST = 8;
constexpr int LCD_BL = 15;

constexpr int TOUCH_SCL = 10;
constexpr int TOUCH_SDA = 11;
constexpr int TOUCH_RST = 13;
constexpr int TOUCH_INT = 14;

constexpr int LCD_WIDTH = 240;
constexpr int LCD_HEIGHT = 280;
constexpr int HEADER_HEIGHT = 34;
constexpr int PAD_TOP = HEADER_HEIGHT;
constexpr int PAD_HEIGHT = LCD_HEIGHT - HEADER_HEIGHT;
constexpr unsigned long TOUCH_DEBOUNCE_MS = 220;

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

CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
MaxSaysGame game;

const uint16_t PAD_COLORS[] = {
  RGB565_RED,
  RGB565_YELLOW,
  RGB565_BLUE,
  RGB565_GREEN
};

const uint16_t PAD_HIGHLIGHTS[] = {
  RGB565_ORANGE,
  RGB565_WHITE,
  RGB565_CYAN,
  RGB565_LIME
};

unsigned long lastTouchMs = 0;
unsigned long lastHeartbeatMs = 0;
bool touchReady = true;

uint16_t colorForZone(Zone zone, bool highlighted) {
  size_t index = static_cast<size_t>(zone);
  if (index >= 4) {
    return RGB565_DARKGREY;
  }
  return highlighted ? PAD_HIGHLIGHTS[index] : PAD_COLORS[index];
}

void drawStatus(const char *line1, const char *line2 = nullptr) {
  gfx->fillRect(0, 0, LCD_WIDTH, HEADER_HEIGHT, RGB565_BLACK);
  gfx->setTextSize(2);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setCursor(8, 7);
  gfx->print(line1);

  if (line2 != nullptr) {
    gfx->setTextSize(1);
    gfx->setCursor(160, 12);
    gfx->print(line2);
  }
}

void drawPads(Zone highlighted = Zone::None) {
  int padWidth = LCD_WIDTH / 2;
  int padHeight = PAD_HEIGHT / 2;

  for (size_t i = 0; i < 4; i++) {
    Zone zone = static_cast<Zone>(i);
    int x = (i % 2) * padWidth;
    int y = PAD_TOP + (i / 2) * padHeight;
    gfx->fillRect(x, y, padWidth, padHeight, colorForZone(zone, zone == highlighted));
    gfx->drawRect(x, y, padWidth, padHeight, RGB565_BLACK);
  }
}

void drawBoard(const char *status, Zone highlighted = Zone::None) {
  char scoreText[18];
  snprintf(scoreText, sizeof(scoreText), "Score %u", static_cast<unsigned>(game.score() - 1));
  drawStatus(status, scoreText);
  drawPads(highlighted);
}

void flashZone(Zone zone, unsigned long holdMs) {
  drawBoard("Watch", zone);
  delay(holdMs);
  drawBoard("Watch");
  delay(160);
}

void addRandomStep() {
  game.appendStep(static_cast<Zone>(random(0, 4)));
}

void startNewGame() {
  game.reset();
  addRandomStep();
}

void playSequence() {
  drawBoard("Max Says");
  delay(650);

  for (size_t i = 0; i < game.sequenceLength(); i++) {
    flashZone(game.sequenceAt(i), 460);
  }

  game.beginInput();
  drawBoard("Your turn");
}

void showGameOver() {
  gfx->fillScreen(RGB565_BLACK);
  gfx->setTextColor(RGB565_RED);
  gfx->setTextSize(3);
  gfx->setCursor(28, 88);
  gfx->println("Game over");

  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(42, 134);
  gfx->printf("Score: %u", static_cast<unsigned>(game.score() - 1));
  gfx->setTextSize(1);
  gfx->setCursor(42, 172);
  gfx->println("Tap anywhere");
}

Zone readTouchedZone() {
  if (!touch.available()) {
    return Zone::None;
  }

  if (touch.data.points == 0 || touch.data.event == 1) {
    touchReady = true;
    return Zone::None;
  }

  if (!touchReady || touch.data.event != 0) {
    return Zone::None;
  }

  touchReady = false;
  int x = touch.data.x;
  int y = touch.data.y;
  if (y < PAD_TOP) {
    return Zone::None;
  }

  Zone zone = zoneFromPoint(x, y - PAD_TOP, LCD_WIDTH, PAD_HEIGHT);
  Serial.printf("Touch down: x=%d y=%d zone=%u\n", x, y, static_cast<unsigned>(zone));
  return zone;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting Max Says");

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed");
    return;
  }

  touch.begin();
  touch.disable_auto_sleep();
  Serial.printf("Touch version: %u\n", touch.data.version);

  randomSeed(esp_random());
  startNewGame();
  playSequence();
}

void loop() {
  if (millis() - lastHeartbeatMs >= 5000) {
    lastHeartbeatMs = millis();
    Serial.println("Max Says heartbeat");
  }

  Zone zone = readTouchedZone();
  if (zone == Zone::None || millis() - lastTouchMs < TOUCH_DEBOUNCE_MS) {
    return;
  }
  lastTouchMs = millis();

  if (game.isGameOver()) {
    startNewGame();
    playSequence();
    return;
  }

  InputResult result = game.enterZone(zone);
  if (result == InputResult::Ignored) {
    return;
  }

  drawBoard("Your turn", zone);
  delay(160);
  drawBoard("Your turn");

  if (result == InputResult::Wrong) {
    Serial.println("Max Says: wrong input");
    showGameOver();
    return;
  }

  if (result == InputResult::RoundComplete) {
    Serial.printf("Max Says: round %u complete\n", static_cast<unsigned>(game.score()));
    drawBoard("Nice!");
    delay(650);
    addRandomStep();
    playSequence();
  }
}
