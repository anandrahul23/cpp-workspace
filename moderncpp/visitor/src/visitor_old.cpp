// #include <iostream>
// #include <vector>
// #include <memory>

// enum class Color
// {
//     Red,
//     Green,
//     Blue
// };

// // Forward declarations
// class Circle;
// class Rectangle;
// class Triangle;

// // Visitor interface

// class ShapeVisitor
// {
// public:
//     virtual void visit(Circle &circle, Color color) = 0;
//     virtual void visit(Rectangle &rectangle, Color color) = 0;
//     virtual void visit(Triangle &triangle, Color color) = 0;
//     virtual ~ShapeVisitor() = default;
// };
// class AreaVisitor : public ShapeVisitor
// {
// public:
//     void visit(Circle &circle, Color color) override
//     {
//         std::cout << "Calculating area of Circle with color " << getColorName(color) << std::endl;
//         area = 3.14159;
//     }

//     void visit(Rectangle &rectangle, Color color) override
//     {
//         std::cout << "Calculating area of Rectangle with color " << getColorName(color) << std::endl;
//     }

//     void visit(Triangle &triangle, Color color) override
//     {
//         std::cout << "Calculating area of Triangle with color " << getColorName(color) << std::endl;
//     }
//     double getArea() const { return area; }
//     double area;
//     std::string getColorName(Color color)
//     {
//         switch (color)
//         {
//         case Color::Red:
//             return "Red";
//         case Color::Green:
//             return "Green";
//         case Color::Blue:
//             return "Blue";
//         default:
//             return "Unknown";
//         }
//     }
// };
// class Shape
// {
// public:
//     virtual void accept(ShapeVisitor &visitor, Color color) = 0;
//     virtual ~Shape() = default;
// };
// // Updated concrete shapes to accept color
// class Circle : public Shape
// {
// public:
//     void accept(ShapeVisitor &visitor, Color color) override
//     {
//         visitor.visit(*this, color);
//     }
// };

// class Rectangle : public Shape
// {
// public:
//     void accept(ShapeVisitor &visitor, Color color) override
//     {
//         visitor.visit(*this, color);
//     }
// };

// class Triangle : public Shape
// {
// public:
//     void accept(ShapeVisitor &visitor, Color color) override
//     {
//         visitor.visit(*this, color);
//     }
// };

// // Updated concrete visitor to draw shapes with different colors
// class DrawShapeVisitor : public ShapeVisitor
// {
// public:
//     void visit(Circle &circle, Color color) override
//     {
//         std::cout << "Drawing Circle with color " << getColorName(color) << std::endl;
//     }

//     void visit(Rectangle &rectangle, Color color) override
//     {
//         std::cout << "Drawing Rectangle with color " << getColorName(color) << std::endl;
//     }

//     void visit(Triangle &triangle, Color color) override
//     {
//         std::cout << "Drawing Triangle with color " << getColorName(color) << std::endl;
//     }

// private:
//     std::string getColorName(Color color)
//     {
//         switch (color)
//         {
//         case Color::Red:
//             return "Red";
//         case Color::Green:
//             return "Green";
//         case Color::Blue:
//             return "Blue";
//         default:
//             return "Unknown";
//         }
//     }
// };

// int main()
// {
//     std::vector<std::unique_ptr<Shape>> shapes;
//     shapes.push_back(std::make_unique<Circle>());
//     shapes.push_back(std::make_unique<Rectangle>());
//     shapes.push_back(std::make_unique<Triangle>());

//     DrawShapeVisitor drawVisitor;

//     for (auto &shape : shapes)
//     {
//         AreaVisitor areaVisitor;
//         shape->accept(drawVisitor, Color::Red); // Example: Drawing all shapes with Red color
//         shape->accept(areaVisitor, Color::Red); // Example: Calculating area of all shapes with Red color
//     }

//     return 0;
// }