#include <memory>
#include <chrono>
#include "Request.h"

class ICommand
{

    protected:
    std::shared_ptr<IRequest> request;
    std::chrono::time_point<std::chrono::steady_clock> requested_time;

    

    public:
 virtual ~ICommand() =0;
 std::shared_ptr<IRequest> getRequest();
 virtual void execute() =0;
 const std::chrono::time_point<std::chrono::steady_clock>& getRequestedTime() const;

 bool operator <<(const ICommand& destCommand) const;

};

class ElevatorCommand : public ICommand
{
    public:
    ~ElevatorCommand(){}
    void execute() override;

};