// #include <iostream>

// #include <vector>
// #include <memory>

// enum class Color
// {
//     Red,
//     Green,
//     Blue
// };

// class Shape
// {
// public:
//     Shape(Color color) : color(color) {}
//     virtual ~Shape() = default;
//     virtual void draw() const = 0;

// protected:
//     Color color;
// };

// class Circle : public Shape
// {
// public:
//     Circle(Color color) : Shape(color) {}
//     void draw() const override
//     {
//         std::cout << "Drawing Circle of color " << static_cast<int>(color) << std::endl;
//     }
// };

// class Square : public Shape
// {
// public:
//     Square(Color color) : Shape(color) {}
//     void draw() const override
//     {
//         std::cout << "Drawing Square of color " << static_cast<int>(color) << std::endl;
//     }
// };

// int main()
// {

//     std::vector<std::unique_ptr<Shape>> shapes;
//     shapes.push_back(std::make_unique<Circle>(Color::Red));
//     shapes.push_back(std::make_unique<Square>(Color::Green));
//     shapes.push_back(std::make_unique<Circle>(Color::Blue));

//     for (const auto &shape : shapes)
//     {
//         shape->draw();
//     }

//     return 0;
// }