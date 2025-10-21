#pragma once
#include <utility>
#include "Classes.h"

// Convert game coordinates (x, y) to array indices (col, row).
// x: 0..width-1 (left -> right)
// y: 0..height-1 (bottom -> top)
// return: {col, row} where row 0 is top row in the printed array
std::pair<int, int> CoorToArr(int x, int y, int boardWidth, int boardHeight);

// Reverse mapping (array indices to game coords)
std::pair<int, int> ArrToCoor(int col, int row, int boardWidth, int boardHeight);

// Validity helpers (kept for compatibility)
bool Valid(const Position& position, const Snake& snake, int boardWidth, int boardHeight);

// Utility to check if head is on any food (not strictly necessary with current design)
bool OnFood(const Snake& snake, const Food& food);
