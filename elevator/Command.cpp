#include "Command.h"


ICommand::~ICommand()
{

}
std::shared_ptr<IRequest> ICommand::getRequest()
{
    return request;
}

const std::chrono::time_point<std::chrono::steady_clock>& ICommand::getRequestedTime() const
{
    return requested_time;
}

bool ICommand::operator <<(const ICommand& destCommand) const
{ 
    return this->getRequestedTime() < destCommand.getRequestedTime();
}

void ElevatorCommand::execute()
{
    int dest_floor = getRequest()->getDestinationFloor();
    int source_floor = getRequest()->getSourceFloor();
}