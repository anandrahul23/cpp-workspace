// #include <iostream>
// #include <vector>
// #include <memory>
// #include <string>
// using namespace std;
// // Strategy interface for drawing

// class Shape;
// class DrawStrategy {
// public:
//     virtual void draw(const Shape& shape) const = 0;
//     virtual ~DrawStrategy() = default;
// };
// // Base class for shapes
// class Shape {
// public:
//     virtual void draw() { drawStrategy->draw(*this); }
//     virtual ~Shape() = default;
//     virtual string getName() const { return name_; }
//     Shape(const string& name) : name_(name) {}
//     Shape(const string& name, unique_ptr<DrawStrategy> st) : name_(name), drawStrategy(std::move(st)) {}
//     private:
//     string name_;
//     unique_ptr<DrawStrategy> drawStrategy;
// };

// // Concrete class for drawing a circle
// class Circle : public Shape {
// public:
//     Circle(const string& name) : Shape(name) {}
//     Circle(const string& name, unique_ptr<DrawStrategy> st) : Shape(name, std::move(st)) {}
   
// };

// // Concrete class for drawing a square
// class Square : public Shape {
// public:
//     Square(const string& name) : Shape(name) {}
//     Square(const string& name, unique_ptr<DrawStrategy> st) : Shape(name, std::move(st)) {}
// };






// // Concrete strategy for drawing a circle
// class GreyDrawStrategy : public DrawStrategy {
// public:
//     void draw(const Shape& shape) const override {
//         std::cout << "Drawing with GreyDrawstrategy " << shape.getName()<<endl;
//     }
// };

// // Concrete strategy for drawing a square
// class ColorDrawStrategy : public DrawStrategy {
// public:
//     void draw(const Shape& shape) const override {
//         std::cout << "Drawing with ColorDrawStrategy " << shape.getName()<< std::endl;
//     }
// };

// // Test strategy for drawing
// class TestDrawStrategy : public DrawStrategy {
// public:
//     void draw(const Shape& shape) const override {
//         std::cout << "Test drawing " << shape.getName() << std::endl;
//     }
// };


// int main() {
//     // Create shapes
//     std::vector<std::unique_ptr<Shape>> shapes;
//         shapes.push_back(std::make_unique<Circle>("Circle", std::make_unique<GreyDrawStrategy>()));
//         shapes.push_back(std::make_unique<Square>("Square", std::make_unique<ColorDrawStrategy>()));
//         shapes.push_back(std::make_unique<Circle>("Circle", std::make_unique<TestDrawStrategy>()));

//     for (const auto& shape : shapes) {
//         shape->draw();
//     }

//     return 0;
// }