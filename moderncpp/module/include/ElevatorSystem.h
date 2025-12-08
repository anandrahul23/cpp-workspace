#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <memory>
#include <optional>
#include <iostream>
#include <exception>
#include <type_traits>
#include <functional>


using namespace std; 

struct MessageBase
{
virtual ~MessageBase() = default; 
};

template <typename T>
struct  MessageWrapper : public MessageBase
{
    T msg;
    MessageWrapper(const T& m): msg(m)
    {

    }
}; 

class MessageBus 
{
public: 
MessageBus() {
}
~MessageBus()
{
    shutDown(); 
}
template<typename T>
void push(T&& val)
{
    if(shut_down.load(memory_order_acquire))
    {
        //throw exception; 
        cout<<"cant push during message bus shut down"<<endl;
        return; 
    }
    scoped_lock<mutex> lk{qMutex}; 
    {
        message_bus.emplace(make_shared<MessageWrapper<T>>(MessageWrapper(std::forward<T>(val)))); 
    }
    consumer_ready.notify_one(); 
}

optional<shared_ptr<MessageBase>> waitAndPop()
{
    unique_lock<mutex> lk{qMutex};
    consumer_ready.wait(lk, [this](){
        return !message_bus.empty() || shut_down.load(memory_order_acquire);
    }); 

    if(message_bus.empty() && shut_down.load(memory_order_acquire))
    {
        return nullopt;
    }

    auto msgPtr =std::move(message_bus.front());
    message_bus.pop();
    return make_optional(msgPtr); 
}
void shutDown()
{
    shut_down.store(true, memory_order_release);
    consumer_ready.notify_all();
}

private:

mutex qMutex; 
condition_variable consumer_ready; 
atomic<bool> shut_down{false}; 
queue<shared_ptr<MessageBase>> message_bus; 
}; 

struct close_message{}; 
class Sender
{
    public:
    Sender():messages_ptr(nullptr) {}
    explicit Sender(shared_ptr<MessageBus> ptrQueue) : messages_ptr(ptrQueue) {}

private:
    shared_ptr<MessageBus> messages_ptr;
};

class BaseDispatcher : public enable_shared_from_this<BaseDispatcher>
{
    public:
        virtual ~BaseDispatcher() = default;
        virtual void run() =0;
        virtual bool dispatchMessage(shared_ptr<MessageBase> msgPtr) =0;
}; 


//main logic for chaining and message passing

// Forward declaration of TemplateDispatcher
template <typename MessageType, typename FuncType>
class TemplateDispatcher;

class Dispatcher : public BaseDispatcher
{

    public: 
    Dispatcher(shared_ptr<MessageBus> queuePtr):messages_ptr(queuePtr)
    {}
    ~Dispatcher()
    {
    }

    void run() override
    {
        handle_messages();
    }

    // Dispatcher(const Dispatcher& ) = delete;
    // Dispatcher& operator=(const Dispatcher&) = delete;

    template <typename MessageType, typename FuncType>
    friend class TemplateDispatcher;

    template <typename MessageType, typename FuncType>
    auto handle(FuncType &&func)
    {
        return TemplateDispatcher<MessageType, FuncType>(this->shared_from_this(), messages_ptr, std::forward<FuncType>(func));
    }

    private: 

    void handle_messages()
    {
        for(;;)
        {
            auto msgPtr = messages_ptr->waitAndPop();
            if (msgPtr)
            {
                if (dispatchMessage(*msgPtr))
                {
                    cout << "message handled" << endl;
                    break;
                }
            }
            else
            {
                //handle shutdown 
                cout<<"shut down received"<<endl;

            }
                
        }
    }

    bool dispatchMessage(shared_ptr<MessageBase> msgPtr) override
    {
        auto cast_msg_ptr = dynamic_pointer_cast<close_message>(msgPtr); 
        if(cast_msg_ptr)
        {
            throw runtime_error("close message received");
        }
        return false; // not handled here , will be handled in template dispatcher
    }

    

    shared_ptr<MessageBus> messages_ptr;
};

template <typename MessageType, typename FuncType>
class TemplateDispatcher
    : public BaseDispatcher
{
    public:
        template <typename OtherMsgType, typename OtherFuncType>
        friend class TemplateDispatcher; // this is for creating object of TemplateDispatcher in handle
        TemplateDispatcher(shared_ptr<BaseDispatcher> prevD, shared_ptr<MessageBus> ptrQueue, FuncType &&fun);
        // TemplateDispatcher(const TemplateDispatcher&) = delete;
        // TemplateDispatcher& operator=(TemplateDispatcher&) = delete;

        void run() override
        {
            handle_messages();
        }

    void handle_messages()
    {
        for (;;)
        {
            auto msgPtr = queuPtr->waitAndPop();
            if (msgPtr)
            {
                if (dispatchMessage(*msgPtr))
                {
                    cout << "message handled" << endl;
                    break;
                }
            }
            else
            {
                // handle shutdown
                cout << "shut down received" << endl;
            }
        }
    }

    bool dispatchMessage(shared_ptr<MessageBase> msgPtr) override
    {
        auto castMsgPtr = dynamic_pointer_cast<MessageWrapper<MessageType>>(msgPtr);
        if(castMsgPtr)
        {
            f_(castMsgPtr->msg);
            return true; //handled message
        }
        else
        {
            return prevDispatcherPtr->dispatchMessage(msgPtr);
        }
    }
    template <typename OtherMsgType, typename OtherFuncType>
    auto handle(OtherFuncType &&fun)
    {
        return TemplateDispatcher< OtherMsgType, OtherFuncType>(this->shared_from_this(), queuPtr, std::forward<OtherFuncType>(fun));
    }

    private : 
    shared_ptr<MessageBus> queuPtr;
    shared_ptr<BaseDispatcher> prevDispatcherPtr;
    FuncType f_; 
    
};

template <typename MessageType, typename FuncType>
TemplateDispatcher<MessageType, FuncType>::TemplateDispatcher(shared_ptr<BaseDispatcher> prevD, shared_ptr<MessageBus> ptrQueue, FuncType &&fun) : prevDispatcherPtr(prevD), queuPtr(ptrQueue), f_(std::forward<FuncType>(fun))
{

}

    class Receiver
{
    public:
    Receiver():ptr_messages_(make_shared<MessageBus>())
    , base_dispatcher(make_shared<Dispatcher>(ptr_messages_))
    {

    }

    operator Sender() 
    {
        return Sender(ptr_messages_);
    }

    template<typename T> 
    void push_message(T&& val)
    {
        if(ptr_messages_)
        {
            ptr_messages_->push(std::forward(val));
        }
       
    }

    shared_ptr<Dispatcher> getDispatcher()
    {
        return base_dispatcher;
    }

    private:

    shared_ptr<MessageBus> ptr_messages_;

    shared_ptr<Dispatcher> base_dispatcher; 

   

};

enum class Direction
{
    UP, DOWN, IDLE 
   
}; 
 ostream &operator<<(ostream& os, Direction dir)
{
    switch (dir)
    {
        case Direction::UP :
                os << "UP";
        break;
        case Direction::DOWN :
            os << "DOWN";
        break;
        case Direction::IDLE :
            os << "IDLE";
            break;
        default:
        os<<"no match";
        break;
    }

return os;
}
enum class DoorState
{
    OPENED, CLOSED, OPENING, CLOSING 
}; 

struct CallButtonPressed
{
    Direction direction_; 
    int floor; 
}; 

struct IntrenalButtonPressed
{
    int floor; 
}; 

class Elevator
{
    public:

    Elevator()
    {
        current_state_fun = [this](){handle_idle_state();}; 
    }

    void runElevator()
    {
        for(;;)
        {
            try
            {
                current_state_fun();
            }
            catch(...)
            {

            }
               
        }
    }

    void handle_idle_state()
    {
        //get current floor, 
        //get direction to travel 
        incoming.getDispatcher()->handle<CallButtonPressed>([&](const CallButtonPressed& arg){

            cout<<"call button pressed with direction:"<<arg.direction_<<" and floor:"<<arg.floor<<endl;

        }).handle<IntrenalButtonPressed>([&](const IntrenalButtonPressed& arg){

        })
        .run();
    }

    private:

    function<void()>current_state_fun;

    Receiver incoming; 
    
}; 




