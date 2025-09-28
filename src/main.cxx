#include <print>
#include <iostream>
#include <array>

constexpr auto width  = 10;
constexpr auto height = 20;

enum class shapes {
    STRAIGHT, SQUARE, T, L, SKEW    
};

typedef std::array<std::array<bool, 4>, 3> Tetromino;

Tetromino get_tetromino(shapes shape) {
    switch (shape) {
        case shapes::STRAIGHT: return { {{true, true, true, true}} };
        case shapes::SQUARE:   return { {{true, true}, {true, true}} };
        case shapes::T:        return { {{true, true, true}, {false, true}} };
        case shapes::L:        return { {{true}, {true}, {true, true}} };
        case shapes::SKEW:     return { {{true}, {true, true}, {false, true}} };
        default: throw std::exception(); // Unreachable
    };
}

void __print_tetromino(Tetromino t) {
    for (const auto& row : t) {
        for (const auto& cell : row)
            std::print(std::cerr, "{}", cell ? "■" : " ");
        std::println(std::cerr, "");
    }
}

int main(void) {
    __print_tetromino(get_tetromino(shapes::STRAIGHT));
    __print_tetromino(get_tetromino(shapes::SQUARE));
    __print_tetromino(get_tetromino(shapes::T));
    __print_tetromino(get_tetromino(shapes::L));
    __print_tetromino(get_tetromino(shapes::SKEW));
    return 0;
}
