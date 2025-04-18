#include <gtest/gtest.h>
#include "modernDraw.h" // Defines Shape, Circle, Square, Point, Color, toString()
#include "../include/circle.h"
#include "../include/square.h"
#include "../include/color.h"
// #include <gmock/gmock.h> // Include Google Mock for matchers like HasSubstr

#include <vector>
#include <string>
#include <iostream> // Include for potential output checking or debugging

// --- Assumptions/Mocks based on main.cpp usage ---
// (You might already have these defined in your actual headers)

// Assume Drawer classes have operator() and store color
struct GlobalDrawer
{
    Color color;
    template <typename T>
    void operator()(const T &shape) const
    {
        std::cout << "GlobalDrawer: Drawing a " << shape.getName()
                  << " with color " << toString(color) << std::endl;
    }
};

struct TestDrawer
{
    Color color;
    template <typename T>
    void operator()(const T &shape) const
    {
        std::cout << "TestDrawer: Drawing a " << shape.getName()
                  << " with color " << toString(color) << std::endl;
    }
};

// --- End Assumptions ---

// Test Fixture for Shape Tests (Optional, but good practice)
class ShapeTest : public ::testing::Test
{
protected:
    // You can set up common objects here if needed
    Point p1{10.0, 10.0};
    Point p2{20.0, 30.0};
};

// Test case for constructing Shape with a Circle and a GlobalDrawer functor
TEST_F(ShapeTest, ConstructWithCircleAndFunctor)
{
    Circle circle{p1, 5.0};
    GlobalDrawer drawer{Color::Red};

    // Check if construction works without error
    EXPECT_NO_THROW(Shape shape(circle, drawer));

    Shape shape(circle, drawer);
    // Check if draw() executes without error
    EXPECT_NO_THROW(shape.draw());
}

// Test case for constructing Shape with a Square and a TestDrawer functor
TEST_F(ShapeTest, ConstructWithSquareAndFunctor)
{
    Square square{p2, 15.0};
    TestDrawer drawer{Color::Green};

    EXPECT_NO_THROW(Shape shape(square, drawer));

    Shape shape(square, drawer);
    EXPECT_NO_THROW(shape.draw());
}

// Test case for constructing Shape with a Circle and a Lambda drawer
TEST_F(ShapeTest, ConstructWithCircleAndLambda)
{
    Circle circle{p1, 25.0};
    bool lambda_called = false;
    Color lambda_color = Color::Blue;

    // Lambda captures a flag to verify it gets called
    auto lambda_drawer = [&lambda_called, lambda_color](const Circle &c)
    {
        std::cout << "LambdaDrawer: Drawing a " << c.getName()
                  << " with color " << toString(lambda_color) << std::endl;
        lambda_called = true;
    };

    EXPECT_NO_THROW(Shape shape(circle, lambda_drawer));

    Shape shape(circle, lambda_drawer);
    EXPECT_FALSE(lambda_called); // Ensure flag is false initially

    EXPECT_NO_THROW(shape.draw());

    // Verify that the lambda was actually executed by draw()
    EXPECT_TRUE(lambda_called);
}

// Test case for storing different Shapes in a vector (polymorphism)
TEST_F(ShapeTest, StoreAndDrawFromVector)
{
    Shape::Shapes shapes; // Assuming Shape::Shapes is std::vector<Shape>
    bool lambda_called_in_vector = false;

    // Add various shapes like in main.cpp
    shapes.emplace_back(Circle{p1, 10.0}, GlobalDrawer{Color::Red});
    shapes.emplace_back(Square{p2, 20.0}, TestDrawer{Color::Green});
    shapes.emplace_back(Circle{{5, 5}, 30.0}, [&lambda_called_in_vector](const Circle &c)
                        {
        lambda_called_in_vector = true;
        std::cout << "Lambda in vector drawing " << c.getName() << std::endl; });

    EXPECT_EQ(shapes.size(), 3);

    // Draw all shapes - primarily checks that the type erasure allows
    // calling draw() correctly on different contained types via the Shape interface.
    EXPECT_NO_THROW({
        for (const auto &shape : shapes)
        {
            shape.draw();
        }
    });

    // Verify the lambda stored in the vector was called
    EXPECT_TRUE(lambda_called_in_vector);
}

// Example of testing output (more advanced, requires helper)
// This often requires redirecting stdout. GoogleTest has utilities,
// or you can implement a simple redirector.

TEST_F(ShapeTest, VerifyDrawOutput)
{
    ::testing::FLAGS_gtest_death_test_style = "threadsafe"; // Needed for some output capturing methods

    Circle circle{p1, 5.0};
    GlobalDrawer drawer{Color::Red};
    Shape shape(circle, drawer);

    // Capture stdout
    testing::internal::CaptureStdout();
    shape.draw();
    std::string output = testing::internal::GetCapturedStdout();

    // Check if the output contains expected text
    EXPECT_NE(output.find("GlobalDrawer"), std::string::npos) << "Output does not contain 'GlobalDrawer'";
    EXPECT_NE(output.find("Circle"), std::string::npos) << "Output does not contain 'Circle'";
    EXPECT_NE(output.find("Red"), std::string::npos) << "Output does not contain 'Red'";
}
