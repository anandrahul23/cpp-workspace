#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <random>
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

// int main()
// {

//     int numPoints = 10;
//     int min = 0;
//     int max = 100;
//     std::vector<Point> points = generateRandomPoints(numPoints, min, max);

//     return 0;
// }