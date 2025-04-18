#ifndef SQUARE_H
#define SQUARE_H
#include "point.h"

class Square {
public:
    Square(const Point& center, double side) : center_(center), side_(side) {}

    Point getCenter() const { return center_; }
    double getSide() const { return side_; }

    void setCenter(const Point& center) { center_ = center; }
    void setSide(double side) { side_ = side; }
    std::string getName() const { return "Square"; }

private:
    Point center_;
    double side_;
};

#endif // SQUARE_H