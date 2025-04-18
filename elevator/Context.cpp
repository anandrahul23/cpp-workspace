#include "Context.h"
#include "State.h"

IContext::~IContext()
{

}

void IContext::setCurrentState(std::shared_ptr<IState> st)
{
    currentState = st;
    currentState->setContext(shared_from_this());
}

ElevatorContext::ElevatorContext(/* args */)
{
}

ElevatorContext::~ElevatorContext()
{
}


