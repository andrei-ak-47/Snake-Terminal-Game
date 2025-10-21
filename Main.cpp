#include <iostream>
#include "Classes.h"
#include "Constants.h"

int main() {
    // Board size from constants; change here if you want another size
    Game game(static_cast<int>(BOARDWIDTH), static_cast<int>(BOARDHEIGHT), 150);

    std::cout << "Snake starting. Use WASD to move. Ctrl+C to quit.\n";
    bool running = true;
    while (running) {
        running = game.run();
    }
    std::cout << "Game over.\n";
    return 0;
}
