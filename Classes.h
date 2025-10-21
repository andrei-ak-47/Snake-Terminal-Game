#pragma once
#include <deque>
#include <vector>
#include <unordered_set>

enum class DIRECTION { RIGHT, UP, LEFT, DOWN };

struct Position {
    int x = 0;
    int y = 0;
    Position() = default;
    Position(int pos_x, int pos_y) : x(pos_x), y(pos_y) {}

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

struct PositionHash {
    std::size_t operator()(const Position& p) const noexcept {
        // combine hashes for x and y
        return (static_cast<std::size_t>(p.x) << 32) ^ static_cast<std::size_t>(p.y);
    }
};

class Food; // forward

class Snake {
public:
    std::deque<Position> body;
    DIRECTION dir = DIRECTION::RIGHT;
    int pending_growth = 0; // number of segments to grow

    Snake();

    // set direction (prevents instant reverse)
    void setDirection(DIRECTION newDir);

    // returns false if collision (wall/self). `ate` set true if snake ate food at new head.
    bool step(int boardWidth, int boardHeight,
              const std::unordered_set<Position, PositionHash>& foodPositions,
              bool &ate);

    Position head() const { return body.front(); }
    bool occupies(const Position& p, bool includeHead = true) const;
};

class Board {
private:
    int width, height;
    std::vector<std::vector<char>> grid;
public:
    Board(int w, int h);

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void clear();
    void drawSnake(const Snake& snake);
    void drawFood(const std::unordered_set<Position, PositionHash>& foods);
    void print() const;
};

class Food {
public:
    std::unordered_set<Position, PositionHash> positions;
    Food() = default;

    // spawn `count` food items (won't place on snake); will attempt limited tries to avoid infinite loop
    void spawn(int count, int boardWidth, int boardHeight, const Snake& snake);
    // remove a food at pos (if present)
    void eatAt(const Position& pos) { positions.erase(pos); }
    int size() const { return static_cast<int>(positions.size()); }
};

class Game {
private:
    Snake snake;
    Board board;
    Food food;
    int frameMilliseconds = 150;
public:
    Game(int boardWidth = 15, int boardHeight = 15, int frameMs = 150);

    // run one frame: returns true if game continues, false if game over
    bool run();
};
