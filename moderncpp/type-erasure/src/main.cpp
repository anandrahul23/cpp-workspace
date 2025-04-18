#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <random>
#include "../include/modernDraw.h"
#include "../include/drawer.h"
#include "../include/Function.h"
#include "../include/circle.h"
#include "../include/square.h"
#include "../include/color.h"
#include "../include/point.h"

std::vector<Point> generateRandomPoints(int numPoints, int min, int max)
{
    std::vector<Point> points;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    for (int i = 0; i < numPoints; ++i)
    {
        points.push_back(Point{static_cast<double>(dis(gen)), static_cast<double>(dis(gen))});
    }

    return points;
}

int main()
{

    int numPoints = 10;
    int min = 0;
    int max = 100;
    std::vector<Point> points = generateRandomPoints(numPoints, min, max);
    Shape::Shapes shapes;

    // shapes.emplace_back(Circle{points[0], 10}, GlobalDrawer{Color::  });
    shapes.emplace_back(Square{points[1], 20}, GlobalDrawer{Color::Green});
    shapes.emplace_back(Circle{points[2], 30}, TestDrawer{Color::Blue});
    shapes.emplace_back(Square{points[3], 40}, TestDrawer{Color::Yellow});
    shapes.emplace_back(Circle{points[4], 50}, [](const Circle &sh)
                        { cout << "Drawing a :" << sh.getName() << " with color " << toString(Color::Black) << " in lambda mode" << endl; });

    for (const auto &shape : shapes)
    {
        shape.draw();
    }

    Function<void(void)> g = &testFunction;

    Function<void(void)> g1 = []()
    { std::cout << "Hello, World!" << std::endl; };

    g();
    g1();
    g1 = g;
    g1();

    return 0;
}