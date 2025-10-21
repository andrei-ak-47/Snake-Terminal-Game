#include "Classes.h"
#include "Constants.h"
#include "ExtraHelpers.h"

#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#ifdef _WIN32
#include <conio.h>
#endif

// ----------------- Snake -----------------
Snake::Snake() : dir(DIRECTION::RIGHT), pending_growth(0) {
    // start near center
    int StartX = 7;
    int StartY = 7;
    // length 3 to start
    for (int i = 0; i < 3; ++i) {
        body.push_back({StartX - i, StartY});
    }
}

void Snake::setDirection(DIRECTION newDir) {
    // prevent reversing into itself
    if ((dir == DIRECTION::LEFT && newDir == DIRECTION::RIGHT) ||
        (dir == DIRECTION::RIGHT && newDir == DIRECTION::LEFT) ||
        (dir == DIRECTION::UP && newDir == DIRECTION::DOWN) ||
        (dir == DIRECTION::DOWN && newDir == DIRECTION::UP)) {
        return;
    }
    dir = newDir;
}

bool Snake::occupies(const Position& p, bool includeHead) const {
    unsigned int idx = 0;
    for (const auto& seg : body) {
        if (!includeHead && idx == 0) { ++idx; continue; }
        if (seg == p) return true;
        ++idx;
    }
    return false;
}

bool Snake::step(int boardWidth, int boardHeight,
                 const std::unordered_set<Position, PositionHash>& foodPositions,
                 bool &ate) {
    // compute new head
    Position newHead = body.front();
    switch (dir) {
        case DIRECTION::RIGHT: newHead.x += 1; break;
        case DIRECTION::LEFT:  newHead.x -= 1; break;
        case DIRECTION::UP:    newHead.y += 1; break;
        case DIRECTION::DOWN:  newHead.y -= 1; break;
    }

    // check wall collision
    if (newHead.x < 0 || newHead.x >= boardWidth ||
        newHead.y < 0 || newHead.y >= boardHeight) {
        return false;
    }

    // check self-collision:
    // If we're going to move without growing, the tail will move away, so allow moving into the tail position.
    Position tail = body.back();
    bool movingIntoTailIsAllowed = (pending_growth == 0);

    if (occupies(newHead, /*includeHead=*/true)) {
        if (!(movingIntoTailIsAllowed && newHead == tail)) {
            return false; // collided with body
        }
    }

    // did we eat?
    ate = (foodPositions.find(newHead) != foodPositions.end());
    if (ate) {
        pending_growth += 1; // grow by 1
    }

    // move: push front
    body.push_front(newHead);
    if (pending_growth > 0) {
        // consume one growth unit (do not pop tail)
        --pending_growth;
    } else {
        // normal move: remove tail
        body.pop_back();
    }

    return true;
}

// ----------------- Board -----------------
Board::Board(int w, int h) : width(w), height(h),
    grid(static_cast<size_t>(h), std::vector<char>(static_cast<size_t>(w), '#')) {
    clear();
}

void Board::clear() {
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c)
            grid[r][c] = '#';
}

void Board::drawSnake(const Snake& snake) {
    for (const auto& seg : snake.body) {
        auto arr = CoorToArr(seg.x, seg.y, width, height);
        int row = arr.second;
        int col = arr.first;
        if (row >= 0 && row < height && col >= 0 && col < width) {
            grid[row][col] = 'O';
        }
    }
}

void Board::drawFood(const std::unordered_set<Position, PositionHash>& foods) {
    for (const auto& p : foods) {
        auto arr = CoorToArr(p.x, p.y, width, height);
        int row = arr.second;
        int col = arr.first;
        if (row >= 0 && row < height && col >= 0 && col < width) {
            grid[row][col] = '@';
        }
    }
}

void Board::print() const {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            std::cout << grid[r][c] << ' ';
        }
        std::cout << '\n';
    }
}

// ----------------- Food -----------------
void Food::spawn(int count, int boardWidth, int boardHeight, const Snake& snake) {
    if (count <= 0) return;

    static std::mt19937 rng(static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> xDist(0, boardWidth - 1);
    std::uniform_int_distribution<int> yDist(0, boardHeight - 1);

    int attempts = 0;
    const int maxAttempts = count * 50 + 500; // upper bound so we don't infinite loop
    while (count > 0 && attempts < maxAttempts) {
        ++attempts;
        Position p{xDist(rng), yDist(rng)};
        // if on snake or already a food location => skip
        if (snake.occupies(p, /*includeHead=*/true)) continue;
        if (positions.find(p) != positions.end()) continue;
        positions.insert(p);
        --count;
    }
}

// ----------------- Game -----------------
Game::Game(int boardWidth, int boardHeight, int frameMs)
    : snake(), board(boardWidth, boardHeight), food(), frameMilliseconds(frameMs) {
    // initial food
    food.spawn(3, board.getWidth(), board.getHeight(), snake);
}

bool Game::run() {
#ifdef _WIN32
    // input: check for key and set direction
    if (_kbhit()) {
        char k = static_cast<char>(_getch());
        switch (k) {
            case 'w': case 'W': snake.setDirection(DIRECTION::UP); break;
            case 's': case 'S': snake.setDirection(DIRECTION::DOWN); break;
            case 'a': case 'A': snake.setDirection(DIRECTION::LEFT); break;
            case 'd': case 'D': snake.setDirection(DIRECTION::RIGHT); break;
            default: break;
        }
    }
#endif

    // attempt one step
    bool ate = false;
    bool alive = snake.step(board.getWidth(), board.getHeight(), food.positions, ate);
    if (!alive) return false;

    // if ate, remove that food and spawn a new one (keep 3 on board)
    if (ate) {
        food.eatAt(snake.head());
    }
    if (food.size() < 3) {
        food.spawn(3 - food.size(), board.getWidth(), board.getHeight(), snake);
    }

    // render
    board.clear();
    board.drawFood(food.positions);
    board.drawSnake(snake);

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    board.print();

    // frame delay
    std::this_thread::sleep_for(std::chrono::milliseconds(frameMilliseconds));
    return true;
}
