#pragma once
#include "point.h"

class Circle
{
public:
    // Getter for center
    Point getCenter() const {
        return center;
    }

    // Setter for center
    void setCenter(const Point& p) {
        center = p;
    }

    // Getter for radius
    double getRadius() const {
        return radius;
    }

    // Setter for radius
    void setRadius(double r) {
        radius = r;
    }

    //getter for name
    std::string getName() const {
        return "Circle";
    }


    
    Circle(Point p, double r) : center(p), radius(r) {}
private:
    Point center;
    double radius;
};