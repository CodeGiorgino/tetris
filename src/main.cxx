#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <print>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <utility>

using namespace std::chrono_literals;

// ----------------------------------------------------------------------------- 
// Constants Definitions
// ----------------------------------------------------------------------------- 

constexpr auto width  = 10;
constexpr auto height = 20;

// ----------------------------------------------------------------------------- 
// Enumerators Definitions
// ----------------------------------------------------------------------------- 

// Tile, the states of a cell
enum class Tile {
    EMPTY, PIVOT, FULL
};

// Shape, the shapes of a tetromino
enum class Shape {
    STRAIGHT, SQUARE, T, L, SKEW, __last
};

// ----------------------------------------------------------------------------- 
// Types Definitions
// ----------------------------------------------------------------------------- 

// Board, stores the board data
typedef std::array<std::array<Tile, width>, height> Board;

// Tetromino, stores the tetromino data
typedef std::array<std::array<Tile, 4>, 3> Tetromino;

// ivec2, stores 2 int components
typedef std::pair<int, int> ivec2;

// ----------------------------------------------------------------------------- 
// Global Variables
// ----------------------------------------------------------------------------- 

Board board {};
ivec2 currentPosition {};
Shape currentShape {};

std::atomic_bool running = true;

// ----------------------------------------------------------------------------- 
// Functions Definitions
// ----------------------------------------------------------------------------- 

// Get the Tetromino from the Shape
Tetromino get_tetromino(Shape shape) {
    switch (shape) {
        case Shape::STRAIGHT: return { {{Tile::FULL, Tile::PIVOT, Tile::FULL, Tile::FULL}} };
        case Shape::SQUARE:   return { {{Tile::PIVOT, Tile::FULL}, {Tile::FULL, Tile::FULL}} };
        case Shape::T:        return { {{Tile::FULL, Tile::PIVOT, Tile::FULL}, {Tile::EMPTY, Tile::FULL}} };
        case Shape::L:        return { {{Tile::FULL}, {Tile::FULL}, {Tile::PIVOT, Tile::FULL}} };
        case Shape::SKEW:     return { {{Tile::FULL}, {Tile::PIVOT, Tile::FULL}, {Tile::EMPTY, Tile::FULL}} };
        default: throw std::exception(); // Unreachable
    };
}

// Draw the UI
void draw() {
    // clear the screen
    std::print("\033[2J\033[1;1H");

    // box boundaries
    std::cout << "┌";
    std::print(std::cout, "{:─>{}}", "", width * 2);
    std::cout << "┐" << std::endl;

    for (auto i = 0; i < board.size(); i++) {
        const auto& row = board[i];
        std::cout << "│";
        for (auto j = 0; j < row.size(); j++) {
            const auto& cell = row[j];
            // TODO: check for current tetromino
            switch (cell) {
                case Tile::EMPTY:
                    {
                        std::cout << "  ";
                        break;
                    }
                case Tile::PIVOT:
                case Tile::FULL:
                    {
                        std::cout << "■■";
                        break;
                    }
                default: throw std::exception(); // Unreachable
            };
        }
        std::cout << "│" << std::endl;
    }

    // box boundaries
    std::cout << "└";
    std::print(std::cout, "{:─>{}}", "", width * 2);
    std::cout << "┘" << std::endl;
}

// Spawn a new tetromino
void spawn() {
    const auto shape = static_cast<Shape>(
            rand() % std::to_underlying(Shape::__last));
}

// Get the key pressed
char get_key_pressed() {
    // set terminal to be non-blocking
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

// Handle the key pressed
void handle_key_press() {
    while (running) {
        switch (get_key_pressed()) {
            case 'h':
                {
                    // TODO: move left
                    break;
                }
            case 'l':
                {
                    // TODO: move right
                    break;
                }
            case 'j':
                {
                    // TODO: move down
                    break;
                }
            case 'r':
                {
                    // TODO: rotate
                    break;
                }
            case 'q':
                {
                    running = false;
                    break;
                }
            default: break;
        };
        std::this_thread::sleep_for(100ms);
    };
}

int main(void) {
    // handle key press in separate thread
    std::thread inputThread(handle_key_press);

    while (running) {
        draw();
        std::this_thread::sleep_for(1s);
    };

    inputThread.join();
    return 0;
}
