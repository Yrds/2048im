#include "raylib.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#include <array>
#include <iostream>
#include <string>
#include <vector>

constexpr auto gridSize = 4;
constexpr auto cellSize = 50;
constexpr Vector2 gridPosition = {10, 45};
constexpr Vector2 windowSize = {20 + gridSize * cellSize, 250};

auto gameOver = false;

using Grid = std::array<std::array<int, gridSize>, gridSize>;

Grid grid = std::array<std::array<int, gridSize>, gridSize>{
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

auto score = 0;

template <typename T> struct Vector2T {
  T x, y;
};

constexpr Rectangle getGridRect() {
  return {gridPosition.x, gridPosition.y, cellSize * gridSize,
          cellSize * gridSize};
}

bool collideCells(Grid &grid, const Vector2T<int> cellA, const Vector2T<int> cellB,
                  const bool onlyTest) {
  if (grid[cellA.x][cellA.y] == grid[cellB.x][cellB.y]) {
    if (!onlyTest) {
      grid[cellA.x][cellA.y] = grid[cellB.x][cellB.y] * 2;
      score += grid[cellA.x][cellA.y];
      grid[cellB.x][cellB.y] = 0;
    }
    return true;
  } else if (grid[cellA.x][cellA.y] == 0 && grid[cellB.x][cellB.y]) {
    if (!onlyTest) {
      grid[cellA.x][cellA.y] = grid[cellB.x][cellB.y];
      grid[cellB.x][cellB.y] = 0;
    }
    return true;
  }

  return false;
}

bool moveHorizontal(Grid &grid, Vector2 direction, bool onlyTest = false) {
  auto moved = false;
  const auto columnStart = direction.x == -1 ? 0 : gridSize - 1;
  const auto columnEnd = direction.x == -1 ? gridSize - 1 : 0;
  const auto columnDirection = direction.x == -1 ? 1 : -1;

  for (auto line = 0; line != gridSize; line++) {
    for (auto column = columnStart; column != columnEnd;
         column += columnDirection) {
      for (auto cell = column + columnDirection;
           cell != columnEnd + columnDirection; cell += columnDirection) {
        if (collideCells(grid, {line, column}, {line, cell}, onlyTest)) {
          moved = true;
        };
        if (grid[line][cell]) {
          break;
        }
      }
    }
  }

  return moved;
}

bool moveVertical(Grid &grid, Vector2 direction, bool onlyTest = false) {
  const auto lineStart = direction.y == -1 ? 0 : gridSize - 1;
  const auto lineEnd = direction.y == -1 ? gridSize - 1 : 0;
  const auto lineDirection = direction.y == -1 ? 1 : -1;

  auto moved = false;

  for (auto column = 0; column != gridSize; column++) {
    for (auto line = lineStart; line != lineEnd; line += lineDirection) {
      for (auto cell = line + lineDirection; cell != lineEnd + lineDirection;
           cell += lineDirection) {
        if (collideCells(grid, {line, column}, {cell, column}, onlyTest)) {
          moved = true;
        };
        if (grid[cell][column]) {
          break;
        }
      }
    }
  }

  return moved;
}

bool moveBlocks(Grid &grid, Vector2 direction, bool onlyTest = false) {
  if (direction.x) {
    return moveHorizontal(grid, direction, onlyTest);
  } else {
    return moveVertical(grid, direction, onlyTest);
  }

  return false;
}

void putRandomBlock(Grid &grid) {
  std::vector<Vector2T<int>> availablePlaces;

  for (auto line = 0; line < gridSize; line++) {
    for (auto column = 0; column < gridSize; column++) {
      if (grid[line][column] == 0) {
        availablePlaces.push_back({line, column});
      }
    }
  }

  if (availablePlaces.size()) {
    auto randomPlace =
        availablePlaces[GetRandomValue(0, availablePlaces.size() - 1)];
    auto randomNumber = GetRandomValue(0, 1);

    grid[randomPlace.x][randomPlace.y] = randomNumber ? 2 : 4;
  }
}

bool testAllSides(Grid &grid) {
  return moveBlocks(grid, {0, 1}, true) || moveBlocks(grid, {0, -1}, true) ||
         moveBlocks(grid, {1, 0}, true) || moveBlocks(grid, {-1, 0}, true);
}

void startNewGame() {
  gameOver = false;
  score = 0;
  grid = {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

  putRandomBlock(grid);
}

void drawScore() {
  const std::string scoreText = "Score: " + std::to_string(score);
  constexpr auto fontSize = 25;
  DrawText(scoreText.c_str(), 10, 10, fontSize, RED);
}

void frame() {
  if (IsKeyPressed(KEY_R)) {
    startNewGame();
  }

  if (gameOver) {

  } else {
    if (!testAllSides(grid)) {
      gameOver = true;
      return;
    }

    Vector2 direction = {0, 0};

    if (IsKeyPressed(KEY_LEFT)) {
      direction = {-1, 0};
    } else if (IsKeyPressed(KEY_RIGHT)) {
      direction = {1, 0};
    } else if (IsKeyPressed(KEY_UP)) {
      direction = {0, -1};
    } else if (IsKeyPressed(KEY_DOWN)) {
      direction = {0, 1};
    }

    if (!(direction.x == 0 && direction.y == 0)) {
      if (moveBlocks(grid, direction)) {
        putRandomBlock(grid);
      };
    }
  }

  // DRAW
  BeginDrawing();

  ClearBackground(RAYWHITE);

  drawScore();

  for (auto i = 0; i < gridSize + 1; i++) {
    const auto linePosition = gridPosition.y + cellSize * i;
    const auto columnPosition = gridPosition.x + cellSize * i;
    DrawLine(gridPosition.x, linePosition, gridPosition.x + cellSize * gridSize,
             linePosition, BLUE);
    DrawLine(columnPosition, gridPosition.y, columnPosition,
             gridPosition.y + cellSize * gridSize, BLUE);
  };

  for (auto line = 0; line < gridSize; line++) {
    for (auto column = 0; column < gridSize; column++) {
      if (grid[line][column] == 0)
        continue;
      constexpr auto fontSize = 25;
      auto numberString = std::to_string(grid[line][column]);
      const auto textMeasure = MeasureText(numberString.c_str(), fontSize);

      const auto linePosition =
          gridPosition.x + cellSize * column +
          (static_cast<int>(cellSize/2) - static_cast<int>(textMeasure/2));
      const auto columnPosition =
          gridPosition.y + cellSize * line +
          (static_cast<int>(cellSize/2) - static_cast<int>(fontSize / 2));

      DrawText(numberString.c_str(), linePosition, columnPosition, fontSize,
               RED);
    }
  }

  if (gameOver) {
    constexpr auto fontSize = 10;
    const std::string text = "GAME OVER ! Press 'R' to restart";
    const auto measuredText = MeasureText(text.c_str(), fontSize);
    const auto gridRectangle = getGridRect();
    DrawRectangle(gridRectangle.x - 1, gridRectangle.y - 1, gridRectangle.width + 1, gridRectangle.height + 1, ColorAlpha({255, 255, 255}, 0.9f));
    DrawText(text.c_str(),
             windowSize.x / 2 - static_cast<int>(measuredText / 2),
             windowSize.y / 2, fontSize, RED);
  }

  EndDrawing();
}

int main(int argc, const char **argv) {
  InitWindow(windowSize.x, windowSize.y, "2048im");

  startNewGame();

  SetTargetFPS(60);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(&frame, 0, 1);
#else
  while (!WindowShouldClose()) {
    frame();
  }
#endif
  CloseWindow();

  return 0;
}
