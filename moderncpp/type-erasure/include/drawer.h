#include "color.h"

class Square;
class Circle;

class GlobalDrawer {

    public:

    GlobalDrawer(Color const& c) : color(c) {}

    void operator()(const Circle& sh) const;
    void operator()(const Square& sh) const;


    private:

    Color color;
};

class TestDrawer {

    public:

    TestDrawer(Color const& c) : color(c) {}

    void operator()(const Circle& sh) const;

    void operator()(const Square& sh) const ;

    private:

    Color color;
};