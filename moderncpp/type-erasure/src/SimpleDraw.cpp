#include <iostream>
#include <memory>
#include <vector>

// Color class
class Color {
public:
    Color(int r, int g, int b) : r(r), g(g), b(b) {}
    void apply() const {
        std::cout << "Applying color (" << r << ", " << g << ", " << b << ")\n";
    }
private:
    int r, g, b;
};

// Point class
class Point {
public:
    Point(int x, int y) : x(x), y(y) {}
    int getX() const { return x; }
    int getY() const { return y; }
private:
    int x, y;
};

// Shape interface
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw(const Color& color) const = 0;
};

// Circle class
class Circle : public Shape {
public:
    Circle(const Point& center, int radius) : center(center), radius(radius) {}
    void draw(const Color& color) const override {
        color.apply();
        std::cout << "Drawing Circle at (" << center.getX() << ", " << center.getY() << ") with radius " << radius << "\n";
    }
private:
    Point center;
    int radius;
};

// Rectangle class
class Rectangle : public Shape {
public:
    Rectangle(const Point& topLeft, int width, int height) : topLeft(topLeft), width(width), height(height) {}
    void draw(const Color& color) const override {
        color.apply();
        std::cout << "Drawing Rectangle at (" << topLeft.getX() << ", " << topLeft.getY() << ") with width " << width << " and height " << height << "\n";
    }
private:
    Point topLeft;
    int width, height;
};

// Drawing function
void drawShapes(const std::vector<std::unique_ptr<Shape>>& shapes, const Color& color) {
    for (const auto& shape : shapes) {
        shape->draw(color);
    }
}

// int main() {
//     std::vector<std::unique_ptr<Shape>> shapes;
//     shapes.push_back(std::make_unique<Circle>(Point(5, 5), 10));
//     shapes.push_back(std::make_unique<Rectangle>(Point(2, 3), 4, 6));

//     Color red(255, 0, 0);
//     drawShapes(shapes, red);

//     return 0;
// }