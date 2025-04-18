#include <iostream>

class AClass
{
private:
    /* data */
public:
    AClass(/* args */);
    ~AClass();
};

AClass::AClass(/* args */)
{
    std::cout<<"ACalss"<<std::endl;
}

AClass::~AClass()
{
}
