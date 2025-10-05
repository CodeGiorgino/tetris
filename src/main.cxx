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

// -----------------------------------------------------------------------------// Constants Definitions
// -----------------------------------------------------------------------------
constexpr auto width  = 10;
constexpr auto height = 20;

#define ANSI_CLEAR            "\033[2J\033[0;0f"
#define ANSI_SAVE_POSITION    "\033[s"
#define ANSI_RESTORE_POSITION "\033[u"

// -----------------------------------------------------------------------------// Enumerators Definitions
// -----------------------------------------------------------------------------
// Tile, the states of a cell
enum class Tile {
    EMPTY, PIVOT, FULL
};

// Shape, the shapes of a tetromino
enum class Shape {
    STRAIGHT, SQUARE, T, L, SKEW, __last
};

// -----------------------------------------------------------------------------// Structures Definitions
// -----------------------------------------------------------------------------
// Point, stores 2 int components
typedef struct {
    int y, x;
} Point;

// -----------------------------------------------------------------------------// Types Definitions
// -----------------------------------------------------------------------------
// Board, stores the board data
typedef std::array<std::array<Tile, width>, height> Board;

// Tetromino, stores the tetromino data
// NOTE: the tiles positions are stored as offsets from the pivot
typedef std::array<std::pair<Tile, Point>, 4> Tetromino;

// -----------------------------------------------------------------------------// Global Variables
// -----------------------------------------------------------------------------
Board board {};
Point currentPosition {};
Tetromino currentTetromino {};

std::atomic_bool running = true;

// -----------------------------------------------------------------------------// Functions Definitions
// -----------------------------------------------------------------------------
// Get the Tetromino from the Shape
Tetromino get_tetromino(Shape shape) {
    switch (shape) {
        case Shape::STRAIGHT:
            return {{
                { Tile::PIVOT, { 0,  0 } },
                { Tile::FULL,  { 0, -1 } },
                { Tile::FULL,  { 0,  1 } },
                { Tile::FULL,  { 0,  2 } }
            }};
        case Shape::SQUARE:             return {{
                { Tile::PIVOT, { 0, 0 } },
                { Tile::FULL,  { 0, 1 } },
                { Tile::FULL,  { 1, 0 } },
                { Tile::FULL,  { 1, 1 } }
            }};
        case Shape::T:                  return {{
                { Tile::PIVOT, { 0,  0 } },
                { Tile::FULL,  { 0, -1 } },
                { Tile::FULL,  { 0,  1 } },
                { Tile::FULL,  { 1,  1 } }
            }};
        case Shape::L:                  return {{
                { Tile::PIVOT, {  0, 0 } },
                { Tile::FULL,  { -1, 0 } },
                { Tile::FULL,  { -2, 0 } },
                { Tile::FULL,  {  0, 1 } }
            }};
        case Shape::SKEW:               return {{
                { Tile::PIVOT, {  0, 0 } },
                { Tile::FULL,  {  1, 0 } },
                { Tile::FULL,  {  0, 1 } },
                { Tile::FULL,  { -1, 0 } }
            }};
        default: throw std::exception(); // Unreachable
    };
}

// Spawn a new tetromino
void spawn_tetromino() {
    currentPosition = { 0, static_cast<int>(width * 0.5) };
    currentTetromino = get_tetromino(
            static_cast<Shape>(
                rand() % std::to_underlying(Shape::__last)));
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

// Update the board
void update() {
    auto newBoard = board;
    bool isColliding = false;
    for (const auto& [_, offset] : currentTetromino) {
        const Point position = {
            currentPosition.y + offset.y + 1,
            currentPosition.x + offset.x
        };

        // check for bottom collision
        if (board[position.y][position.x] == Tile::FULL
                || position.y >= height) {
            isColliding = true;
        }

        newBoard[currentPosition.y + offset.y]
            [currentPosition.x + offset.x]
                = Tile::FULL;
    }

    if (isColliding) {
        std::swap(board, newBoard);
        spawn_tetromino();
    } else {
        currentPosition.y++;
    }

    // clear completed rows
    for (auto i = height - 1; i >= 0; i--) {
        auto& row = board[i];
        bool isComplete = true;
        for (const auto& cell : row) {
            isComplete &= cell != Tile::EMPTY;
        }

        if (isComplete) {
            // move down rows
            for (auto j = i; j > 0; j--) {
                board[j] = board[j - 1];
            }

            board[0] = {};
        }
    }
}

// Handle the key pressed
void handle_key_press() {
    while (running) {
        // TODO: maybe move the logic in the update function
        // to prevent multiplle input between redraws
        switch (get_key_pressed()) {
            case 'h':
                {
                    auto leftmostPosition = currentPosition.x;
                    for (const auto& [_, offset] : currentTetromino) {
                        const auto position = currentPosition.x + offset.x;
                        if (position < leftmostPosition) {
                            leftmostPosition = position;
                        }
                    }

                    if (leftmostPosition > 0) {
                        currentPosition.x--;
                    }
                    break;
                }
            case 'l':
                {
                    auto rightmostPosition = currentPosition.x;
                    for (const auto& [_, offset] : currentTetromino) {
                        const auto position = currentPosition.x + offset.x;
                        if (position > rightmostPosition) {
                            rightmostPosition = position;
                        }
                    }

                    if (rightmostPosition < width - 1) {
                        currentPosition.x++;
                    }
                    break;
                }
            case 'j':
                {
                    while (currentPosition.y != 0) {
                        update();
                    };
                    break;
                }
            case 'r':
                {
                    for (auto& [_, offset] : currentTetromino) {
                        offset = { offset.y, -offset.x };
                    }
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

// Draw the UI
void draw() {
    std::cout << ANSI_CLEAR;

    // draw the box boundaries
    std::println(std::cout,
            "┌{:─>{}}┐",            "", width);

    // draw the board
    for (auto i = 0; i < board.size(); i++) {
        const auto& row = board[i];
        std::print("│");
        for (auto j = 0; j < row.size(); j++) {
            const auto& cell = row[j];
            if (cell == Tile::EMPTY) {
                std::print(" ");
            } else {
                std::print("■");
            }
        }
        std::println("│");
    }

    // draw the box boundaries
    std::println(std::cout,
            "└{:─>{}}┘",
            "", width);

    // draw the current Tetromino
    for (const auto& [_, offset] : currentTetromino) {
        const Point position = {
            currentPosition.y + offset.y,
            currentPosition.x + offset.x
        };

        std::print(std::cout,
                "\033[{};{}f""■",
                position.y + 2,
                position.x + 2);
    }

    std::println(std::cout,
            "\033[{};0f",
            height + 2);
    std::cout << std::flush;
}

int main(void) {
    // handle key press in separate thread
    std::thread inputThread(handle_key_press);

    spawn_tetromino();

    while (running) {
        draw();
        update();
        std::this_thread::sleep_for(1s);
    };

    inputThread.join();
    return 0;
}
