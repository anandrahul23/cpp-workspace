#include "../include/drawer.h"
#include "../include/circle.h"
#include "../include/square.h"
#include <iostream>

using namespace std;
void GlobalDrawer::operator()(const Circle& sh) const {
    cout << "Drawing a circle with radius:"<<sh.getRadius() <<" with color " << toString(color) << endl;
}

void GlobalDrawer::operator()(const Square& sh) const {
    cout << "Drawing a square with center:"<<sh.getSide() <<" with color " << toString(color) << endl;
}

void TestDrawer::operator()(const Circle& sh) const {
    cout << "Drawing a circle with radius:"<<sh.getRadius() <<" with color "<< " in test mode" << endl;
}
void TestDrawer::operator()(const Square& sh) const {
    cout << "Drawing a square with side:"<<sh.getSide() <<" with color "<< " in test mode" << endl;
}


