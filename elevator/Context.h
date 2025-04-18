#ifndef _COTEXT_H___
#define _COTEXT_H___

#include <memory>
class IState;

class IContext : public std::enable_shared_from_this<IContext>
{
    private:
    std::shared_ptr<IState> currentState;
    public:
    virtual ~IContext()=0;
    void setCurrentState(std::shared_ptr<IState> st);
};



class ElevatorContext : IContext
{
private:
    /* data */
   
public:
    ElevatorContext(/* args */);
    ~ElevatorContext();

    
};

#endif
