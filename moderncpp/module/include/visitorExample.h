// #pragma once
// #include  <memory>
// #include <iostream>

// using namespace std; 

// class ShapeVistor; 


// class Shape
// {

//     public: 
//     virtual void accept(shared_ptr<ShapeVistor>  visitor) = 0;  
//     virtual ~Shape() = 0; 
    
// };

// class Circle: public Shape
// {
//     public:
//         void accept(shared_ptr<ShapeVistor> visitor) override
//         {
//             visitor->visit(*this);
//         }
// };

// class Triangle : public Shape
// {
//     public:
//     void accept(shared_ptr<ShapeVistor> visitor) override
//     {
//         visitor->visit(*this);
//     }
// };

// class ShapeVistor
// {
//     public: 
//     //virtual void visit(const Shape &shape) = 0; 
//     virtual void visit(const Circle& circle) = 0;

//     virtual void visit(const Triangle &circle) = 0;
// };

// class DrawShapeVistor : public ShapeVistor
// {
// public:
//     virtual void visit(const Shape &shape)
//     {
//         cout << "Drawing shape base implementation " << endl;
//     }
//     virtual void visit(const Circle &circle){
//         cout<<"Drawing circle"<<endl;
//     }

//     virtual void visit(const Triangle &circle)
//     {
//         cout << "Drawing Triangle" << endl;
//     }
// };

// // void Shape::accept(shared_ptr <ShapeVistor> visitor)
// // {
// //    visitor->visit(*this);   this doesnt work as function arguments are resolved at compile time rather than run time
// // }