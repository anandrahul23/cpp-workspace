// #include <iostream>
// #include <memory>
// #include <vector>
// #include <cstdlib>
// #include <ctime>

// namespace Color
// {
//     enum class ColorType
//     {
//         Red,
//         Green,
//         Blue
//     };
// }

// std::string colorToString(Color::ColorType color)
// {
//     switch (color)
//     {
//     case Color::ColorType::Red:
//         return "Red";
//     case Color::ColorType::Green:
//         return "Green";
//     case Color::ColorType::Blue:
//         return "Blue";
//     default:
//         return "Unknown";
//     }
// }

// class Shape
// {
// public:
//     enum class ShapeType
//     {
//         Circle,
//         Rectangle,
//         Triangle
//     };

//     Shape(ShapeType type) : type_(type) {}
//     virtual ~Shape() = default;

//     ShapeType getType() const { return type_; }

// private:
//     ShapeType type_;
// };

// class Circle : public Shape
// {
// public:
//     Circle() : Shape(ShapeType::Circle) {}
// };

// class Rectangle : public Shape
// {
// public:
//     Rectangle() : Shape(ShapeType::Rectangle) {}
// };

// class Triangle : public Shape
// {
// public:
//     Triangle() : Shape(ShapeType::Triangle) {}
// };

// void draw(const Circle &circle, Color::ColorType color = Color::ColorType::Red)
// {
//     std::cout << "Drawing Circle in color " << colorToString(color) << std::endl;
// }

// void draw(const Rectangle &rectangle, Color::ColorType color = Color::ColorType::Green)
// {
//     std::cout << "Drawing Rectangle in color " << colorToString(color) << std::endl;
// }

// void draw(const Triangle &triangle, Color::ColorType color = Color::ColorType::Blue)
// {
//     std::cout << "Drawing Triangle in color " << colorToString(color) << std::endl;
// }

// Color::ColorType getRandomColor()
// {
//     static bool seeded = false;
//     if (!seeded)
//     {
//         std::srand(static_cast<unsigned int>(std::time(0)));
//         seeded = true;
//     }
//     int randomValue = std::rand() % 3;
//     switch (randomValue)
//     {
//     case 0:
//         return Color::ColorType::Red;
//     case 1:
//         return Color::ColorType::Green;
//     case 2:
//         return Color::ColorType::Blue;
//     default:
//         return Color::ColorType::Red; // Default case, though it should never reach here
//     }
// }

// void draw(const Shape &shape)
// {
//     Color::ColorType color = getRandomColor();
//     switch (shape.getType())
//     {
//     case Shape::ShapeType::Circle:
//         draw(static_cast<const Circle &>(shape), color);
//         break;
//     case Shape::ShapeType::Rectangle:
//         draw(static_cast<const Rectangle &>(shape), color);
//         break;
//     case Shape::ShapeType::Triangle:
//         draw(static_cast<const Triangle &>(shape), color);
//         break;
//     default:
//         std::cout << "Unknown shape" << std::endl;
//         break;
//     }
// }

// int main()
// {
//     std::vector<std::unique_ptr<Shape>> shapes;
//     shapes.push_back(std::make_unique<Circle>());
//     shapes.push_back(std::make_unique<Rectangle>());
//     shapes.push_back(std::make_unique<Triangle>());

//     for (const auto &shape : shapes)
//     {
//         // shape->draw();
//         draw(*shape);
//     }

//     return 0;
// }