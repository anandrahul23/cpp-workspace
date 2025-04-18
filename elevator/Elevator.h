#include <deque>
#include <memory>
class ICommand;
class IState;
class IContext

using namespace std;
class Elevator
{

    private: 
    deque<shared_ptr<ICommand> > currentJobs;
    deque<shared_ptr<ICommand> > upPendingJobs;
    deque<shared_ptr<ICommand> > downPendingJobs;
    shared_ptr<IState> currentState;
    shared_ptr<IContext> elevatorContext;


    public: 
    void addNewRequests();
    void processRequest();
    bool checkIfJobsAvailbale();

};
