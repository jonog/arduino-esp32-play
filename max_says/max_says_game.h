#ifndef MAX_SAYS_GAME_H
#define MAX_SAYS_GAME_H

#include <cstddef>
#include <cstdint>

enum class Zone : uint8_t {
  TopLeft = 0,
  TopRight = 1,
  BottomLeft = 2,
  BottomRight = 3,
  None = 255
};

enum class InputResult : uint8_t {
  Ignored,
  Correct,
  RoundComplete,
  Wrong
};

inline Zone zoneFromPoint(int x, int y, int width, int height) {
  if (x < 0 || y < 0 || x >= width || y >= height) {
    return Zone::None;
  }

  bool right = x >= width / 2;
  bool bottom = y >= height / 2;

  if (!right && !bottom) {
    return Zone::TopLeft;
  }
  if (right && !bottom) {
    return Zone::TopRight;
  }
  if (!right && bottom) {
    return Zone::BottomLeft;
  }
  return Zone::BottomRight;
}

class MaxSaysGame {
public:
  static constexpr size_t MaxSequenceLength = 32;

  void reset() {
    sequenceLength_ = 0;
    inputIndex_ = 0;
    acceptingInput_ = false;
    gameOver_ = false;
  }

  bool appendStep(Zone zone) {
    if (zone == Zone::None || sequenceLength_ >= MaxSequenceLength) {
      return false;
    }
    sequence_[sequenceLength_++] = zone;
    inputIndex_ = 0;
    acceptingInput_ = false;
    gameOver_ = false;
    return true;
  }

  void beginInput() {
    inputIndex_ = 0;
    acceptingInput_ = sequenceLength_ > 0 && !gameOver_;
  }

  InputResult enterZone(Zone zone) {
    if (!acceptingInput_ || gameOver_ || zone == Zone::None) {
      return InputResult::Ignored;
    }

    if (sequence_[inputIndex_] != zone) {
      acceptingInput_ = false;
      gameOver_ = true;
      return InputResult::Wrong;
    }

    inputIndex_++;
    if (inputIndex_ >= sequenceLength_) {
      acceptingInput_ = false;
      return InputResult::RoundComplete;
    }

    return InputResult::Correct;
  }

  Zone sequenceAt(size_t index) const {
    if (index >= sequenceLength_) {
      return Zone::None;
    }
    return sequence_[index];
  }

  size_t sequenceLength() const {
    return sequenceLength_;
  }

  size_t score() const {
    return sequenceLength_;
  }

  bool isGameOver() const {
    return gameOver_;
  }

private:
  Zone sequence_[MaxSequenceLength] = {};
  size_t sequenceLength_ = 0;
  size_t inputIndex_ = 0;
  bool acceptingInput_ = false;
  bool gameOver_ = false;
};

#endif
