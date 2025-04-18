#ifndef _STATE_H__
#define _STATE_H__

#include <memory>

class IContext;


class IState
{
private:
    /* data */
    std::shared_ptr<IContext> context;
public:
    IState(/* args */);
    virtual ~IState() =0 ;

    void setContext(std::shared_ptr<IContext> ctx);
    virtual void move(int src_fllor, int dest_floor) =0; 
    virtual void closeGate() =0;
    virtual void openGate() =0;
};

class ElevatorIdleState : public IState
{

    public:
    ElevatorIdleState();
    ~ElevatorIdleState();

    void move(int src_fllor, int dest_floor) override; 
    void closeGate() override;
    void openGate() override ;

};


#endif