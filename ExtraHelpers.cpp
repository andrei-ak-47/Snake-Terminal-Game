#include "ExtraHelpers.h"
#include <utility>

// Convert coordinates to array indexes used by Board's grid (row major).
// We want (0,0) game coordinate to be bottom-left.
// grid row 0 is top -> row = boardHeight - 1 - y
std::pair<int,int> CoorToArr(int x, int y, int boardWidth, int boardHeight) {
    int col = x;
    int row = boardHeight - 1 - y;
    return {col, row};
}

std::pair<int,int> ArrToCoor(int col, int row, int boardWidth, int boardHeight) {
    int x = col;
    int y = boardHeight - 1 - row;
    return {x, y};
}

bool Valid(const Position& position, const Snake& /*snake*/, int boardWidth, int boardHeight) {
    if (position.x < 0 || position.x >= boardWidth) return false;
    if (position.y < 0 || position.y >= boardHeight) return false;
    return true;
}

bool OnFood(const Snake& snake, const Food& food) {
    return food.positions.find(snake.head()) != food.positions.end();
}
