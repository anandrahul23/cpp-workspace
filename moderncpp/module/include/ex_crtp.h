#pragma once 
#include <memory>
#include <iostream>
using namespace std;

template <typename Derived> 
class Cloneable
{

    public: 
    unique_ptr<Derived>  clone()
    {
        return make_unique<Derived>(static_cast<Derived&>(*this));
    }

    void draw()
    {
        cout<<"drawing:" <<(static_cast<Derived&>(*this)).draw()<<endl;
    }

};

class Circle : public Cloneable<Circle>
{
        public:
        void draw()
        {
                cout<<"drawing circle"<<endl;
        }

};