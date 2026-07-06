#include <cassert>
#include <cstddef>
#include <cstdint>

#include "../max_says/max_says_game.h"

void testPointToZoneUsesQuadrants() {
  assert(zoneFromPoint(10, 10, 240, 280) == Zone::TopLeft);
  assert(zoneFromPoint(200, 10, 240, 280) == Zone::TopRight);
  assert(zoneFromPoint(10, 220, 240, 280) == Zone::BottomLeft);
  assert(zoneFromPoint(200, 220, 240, 280) == Zone::BottomRight);
}

void testRoundCompletesAfterMatchingFullSequence() {
  MaxSaysGame game;
  game.reset();
  assert(game.appendStep(Zone::TopLeft));
  assert(game.appendStep(Zone::BottomRight));
  game.beginInput();

  assert(game.enterZone(Zone::TopLeft) == InputResult::Correct);
  assert(game.enterZone(Zone::BottomRight) == InputResult::RoundComplete);
  assert(game.score() == 2);
  assert(!game.isGameOver());
}

void testWrongInputEndsGame() {
  MaxSaysGame game;
  game.reset();
  assert(game.appendStep(Zone::TopRight));
  game.beginInput();

  assert(game.enterZone(Zone::BottomLeft) == InputResult::Wrong);
  assert(game.isGameOver());
}

int main() {
  testPointToZoneUsesQuadrants();
  testRoundCompletesAfterMatchingFullSequence();
  testWrongInputEndsGame();
}
