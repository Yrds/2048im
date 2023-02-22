#include "raylib.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#include <array>
#include <string>
#include <iostream>

constexpr auto gridSize = 4;
constexpr auto cellSize = 50;
constexpr Vector2 windowSize = {800, 450};
constexpr Vector2 gridPosition = {100, 100};

using Grid =std::array<std::array<int, gridSize>, gridSize>;

Grid grid = std::array<std::array<int, gridSize>, gridSize> {{
    {2, 2, 2, 2},
    {0, 4, 0, 2},
    {0, 2, 0, 2},
    {0, 2, 4, 8}
}};

void moveHorizontal(Grid& grid, Vector2 direction) {
  const auto columnStart = direction.x == -1 ? 0 : gridSize;
  const auto columnEnd = direction.x == -1 ? gridSize - 1 : 0;
  const auto columnDirection = direction.x == -1 ? 1 : -1;

  for (auto line = 0; line != gridSize; line ++) {
    for (auto column = columnStart; column != columnEnd; column += columnDirection) {
      for (auto cell = column + columnDirection; cell != columnEnd + columnDirection; cell += columnDirection) {
        if (grid[line][column] == grid[line][cell]) {
          grid[line][column] = grid[line][cell] * 2;
          grid[line][cell] = 0;
        } else if (grid[line][column] == 0 && grid[line][cell]) {
          grid[line][column] = grid[line][cell];
          grid[line][cell] = 0;
        }
      }
    }
  }
}

void moveVertical(Grid& grid, Vector2 direction) {
  const auto columnStart = direction.x == -1 ? 1 : gridSize - 1;
  const auto columnEnd = direction.x == -1 ? gridSize - 1 : 1;
  const auto columnDirection = direction.x == -1 ? 1 : -1;

  for (auto line = 0; line != gridSize; line ++) {
    for (auto column = columnStart; column != columnEnd; column += columnDirection) {
      for (auto cell = column + columnDirection; cell != columnEnd; cell += columnDirection) {
        if (grid[line][column] == grid[line][cell]) {
          grid[line][column] = grid[line][cell] * 2;
          grid[line][cell] = 0;
        } else if (grid[line][column] == 0 && grid[line][cell]) {
          grid[line][column] = grid[line][cell];
          grid[line][cell] = 0;
        }
      }
    }
  }
}

void moveBlocks(Grid &grid, Vector2 direction) {
  if(direction.x) {
    moveHorizontal(grid, direction);
  } else {
  }
}

void frame() {
    Vector2 direction = {0, 0};

    if(IsKeyPressed(KEY_LEFT)) {
        direction = {-1, 0};
    } else if(IsKeyPressed(KEY_RIGHT)) {
        direction = {1, 0};
    } else if(IsKeyPressed(KEY_UP)) {
        direction = {0, -1};
    } else if (IsKeyPressed(KEY_DOWN)) {
        direction = {0, 1};
    }

    if(!(direction.x == 0 && direction.y == 0)) {
        moveBlocks(grid, direction);
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    for(auto i = 0; i < gridSize + 1; i++) {
        const auto linePosition = gridPosition.y + cellSize * i;
        const auto columnPosition = gridPosition.x + cellSize * i;
        DrawLine(gridPosition.x, linePosition, gridPosition.x + cellSize * gridSize, linePosition, BLUE);
        DrawLine(columnPosition, gridPosition.y, columnPosition, gridPosition.y + cellSize * gridSize, BLUE);
    };

    for(auto line = 0; line < gridSize; line++) {
        for(auto column = 0; column < gridSize; column++) {
            if(grid[line][column] == 0) continue;
            constexpr auto fontSize = 25;
            auto numberString = std::to_string(grid[line][column]);
            const auto textMeasure = MeasureText(numberString.c_str(), fontSize);

            const auto linePosition = gridPosition.y + cellSize * column + (cellSize/2 - textMeasure/2);
            const auto columnPosition = gridPosition.x + cellSize * line + (cellSize/2 - textMeasure/2);

            DrawText(numberString.c_str(), linePosition, columnPosition, fontSize, RED);
        }
    }

    EndDrawing();
}

int main(int argc, const char **argv) {
    InitWindow(windowSize.x, windowSize.y, "2048im");

    SetTargetFPS(60);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&frame, 0, 1);
#else
    while(!WindowShouldClose()) {
        frame();
    }
#endif
    CloseWindow();

    return 0;
}
