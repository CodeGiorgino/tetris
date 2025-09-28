#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <print>
#include <termios.h>
#include <thread>
#include <unistd.h>

using namespace std::chrono_literals;

constexpr auto width  = 10;
constexpr auto height = 20;

enum class tiles {
    EMPTY, PIVOT, FULL
};

enum class shapes {
    STRAIGHT, SQUARE, T, L, SKEW    
};

typedef std::array<std::array<tiles, 4>, 3> Tetromino;
typedef std::array<std::array<tiles, width>, height> Board;

Tetromino get_tetromino(shapes shape) {
    switch (shape) {
        case shapes::STRAIGHT: return { {{tiles::FULL, tiles::PIVOT, tiles::FULL, tiles::FULL}} };
        case shapes::SQUARE:   return { {{tiles::PIVOT, tiles::FULL}, {tiles::FULL, tiles::FULL}} };
        case shapes::T:        return { {{tiles::FULL, tiles::PIVOT, tiles::FULL}, {tiles::EMPTY, tiles::FULL}} };
        case shapes::L:        return { {{tiles::FULL}, {tiles::FULL}, {tiles::PIVOT, tiles::FULL}} };
        case shapes::SKEW:     return { {{tiles::FULL}, {tiles::PIVOT, tiles::FULL}, {tiles::EMPTY, tiles::FULL}} };
        default: throw std::exception(); // Unreachable
    };
}

void __print_tetromino(Tetromino t) {
    for (const auto& row : t) {
        for (const auto& cell : row)
            switch (cell) {
                case tiles::EMPTY: std::print(std::cerr, " ");
                case tiles::PIVOT: std::print(std::cerr, "@");
                case tiles::FULL:  std::print(std::cerr, "#");
                default: throw std::exception(); // Unreachable
            };
        std::println(std::cerr, "");
    }
}

char get_key_pressed() {
    char buf;
    struct termios term;
    if (tcgetattr(0, &term) < 0) {
        std::perror("tcsetattr()");
    }

    term.c_lflag &= ~ICANON;
    term.c_lflag &= ~ECHO;
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &term) < 0) {
        std::perror("tcsetattr ICANON");
    } else if (read(0, &buf, 1) < 0) {
        std::perror("read()");
    }

    term.c_lflag |= ICANON;
    term.c_lflag |= ECHO;

    if (tcsetattr(0, TCSADRAIN, &term) < 0) {
        perror("tcsetattr ~ICANON");
    }

    return buf;
}

Board board;
std::atomic_bool running = true;

void handle_key_press() {
    while (running) {
        switch (get_key_pressed()) {
            case 'q':
                {
                    running = false;
                    break;
                }
            default: break;
        };
    };
}

int main(void) {
    // handle key press in separate thread
    std::thread inputThread(handle_key_press);

    while (running) {
        std::println(std::cerr, "[d] Drawing");
        std::this_thread::sleep_for(1s);
    };

    inputThread.join();
    return 0;
}
