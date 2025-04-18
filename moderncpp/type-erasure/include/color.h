#ifndef COLOR_H
#define COLOR_H

#include <string>

enum class Color {
    Red,
    Green,
    Blue,
    Yellow,
    Black,
    White
};

inline std::string toString(Color color) {
    switch (color) {
        case Color::Red: return "Red";
        case Color::Green: return "Green";
        case Color::Blue: return "Blue";
        case Color::Yellow: return "Yellow";
        case Color::Black: return "Black";
        case Color::White: return "White";
        default: return "Unknown";
    }
}

#endif // COLOR_H