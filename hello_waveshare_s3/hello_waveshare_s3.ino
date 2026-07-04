// Minimal connectivity sketch for a Waveshare ESP32-S3 touch display board.
// It verifies upload, USB serial, flash size, and PSRAM availability.

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000) {
    delay(10);
  }

  Serial.println();
  Serial.println("Hello from Waveshare ESP32-S3 1.69inch Touch Display!");
  Serial.printf("Chip model: %s\n", ESP.getChipModel());
  Serial.printf("Chip revision: %u\n", ESP.getChipRevision());
  Serial.printf("Flash size: %u bytes\n", ESP.getFlashChipSize());
  Serial.printf("PSRAM size: %u bytes\n", ESP.getPsramSize());
  Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
}

void loop() {
  static unsigned long counter = 0;

  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(750);

  Serial.printf("hello tick %lu, uptime %lu ms\n", counter++, millis());
}
