// #include "../include/color.h"
// #include "../include/circle.h"
// #include "../include/point.h"
// #include "../include/square.h"

// #include <variant>
// #include <iostream>

// using Shape = std::variant<Circle, Point, Square>;

// using Shapes = std::vector<Shape>;

// struct DrawVisitor
// {
//     void operator()(const Circle &circle) const
//     {
//         std::cout << "Drawing Circle with radius: " << circle.getRadius() << std::endl;
//     }

//     void operator()(const Point &point) const
//     {
//         std::cout << "Drawing Point at (" << point.getX() << ", " << point.getY() << ")" << std::endl;
//     }

//     void operator()(const Square &square) const
//     {
//         std::cout << "Drawing Sqaure with side : (" << square.getSide() << ")" << std::endl;
//     }
// };

// void drawShape(const Shape &shape)
// {
//     std::visit(DrawVisitor{}, shape);
// }

// struct AreaVisitor
// {
//     double operator()(const Circle &circle) const
//     {
//         return 3.14159 * circle.getRadius() * circle.getRadius();
//     }

//     double operator()(const Point &) const
//     {
//         return 0.0;
//     }

//     double operator()(const Square &square) const
//     {
//         return square.getSide() * square.getSide();
//     }
// };

// void printArea(const Shape &shape)
// {
//     double area = std::visit(AreaVisitor{}, shape);
//     std::cout << "Area: " << area << std::endl;
// }

// int main()
// {
//     Shapes shapes;
//     shapes.emplace_back(Circle(Point(1.0, 2.0), 2.0));
//     shapes.emplace_back(Point(1.0, 2.0));
//     shapes.emplace_back(Square(Point(1.0, 2.0), 2.0));

//     for (const auto &shape : shapes)
//     {
//         drawShape(shape);
//         printArea(shape);
//     }

//     return 0;
// }