#include "State.h"
#include "Context.h"
#include <iostream>

IState::IState(/* args */)
{
}

IState::~IState()
{
}

void IState::setContext(std::shared_ptr<IContext> ctx)
{
    context = ctx;
}



ElevatorIdleState::ElevatorIdleState()
{

}
ElevatorIdleState::~ElevatorIdleState(){}

void ElevatorIdleState::move(int src_floor, int dest_floor)
{
    std::cout<<"Moving from floor:"<<src_floor<<" to floor:"<<dest_floor<<std::endl;
}

void ElevatorIdleState::closeGate()
{
     std::cout<<"closing gate in state:ElevatorIdleState"<<std::endl;
}

void ElevatorIdleState::openGate()
{
    std::cout<<"opening gate in state:ElevatorIdleState"<<std::endl;
}
