#pragma once
#include <memory>


using namespace std; 

//Strategy will have interface for operation , like draw startegy will have draw , will represent this 
//pure virtual function in Concept

struct Concept
{
    virtual void draw() const= 0;
    virtual std::unique_ptr<Concept> clone() const = 0;
    virtual ~Concept() = default;
}; 

//there will be a model for Concept , which will implement this draw behaviour , working on Type T 
//and StrategyT 

template <typename T, typename StrategyT> 
struct Model : public Concept
{
    //provide lvalue and rvalue constructor 
    Model(T obj, StrategyT st):strategy_{make_unique<StrategyT>(std::move(st))}
                                            ,object_{make_unique<T>(std::move(obj))}
    {
         cout << endl<< "Normal l value contrcutor called : Model&" << endl;
    }

     //provide lvalue and rvalue constructor 
    // Model(const T&& obj, const StrategyT&& st): object_{make_unique<T>(std::move(obj))}
    //                             , strategy_{make_unique<StrategyT>(std::move(st))}

    // {
        
    //     cout<<endl<<"R value contrcutor called : Model&&"<<endl;
    // }

    //copy Constructor
    Model(const Model &other) : strategy_{make_unique<StrategyT>(*other.strategy_)}
                , object_{make_unique<T>(*other.object_)}
    {
    }

    Model& operator=(Model other) //other copy constructed
    {
        //copy and swap idiom 
        //Model tmp(other); //copy constructor called 
        std::swap(other.strategy_, strategy_);
        std::swap(other.object_, object_); 
        return *this; 
    }

    Model(Model&& other) = default; //because of unique_ptr

    Model &operator=(Model&& other) =default;  //because of unique_ptr

    unique_ptr<Concept> clone() const override
    {
        return make_unique<Model<T, StrategyT>>(*this); 
    }

    void draw() const override
    {
        (*strategy_)(*object_); 
    }

    private: 
    std::unique_ptr<StrategyT> strategy_; 
    std::unique_ptr<T> object_; 

}; 

//this is where type erasure happens , 
//it will haave templatized  constructor on Type T and Strategy T 
class ModelHolder
{
    public:
    //use universal reference
    //type erasure

        template <typename T, typename StrategyT>
        ModelHolder(T model, StrategyT st) : pimpl_{make_unique<Model<T, StrategyT>>(std::move(model), std::move(st))}
        {
            // No code needed here
        }
    // template<typename ModelT, typename StrategyT> 
    // ModelHolder(ModelT&& model, StrategyT&& st)
                                   
    // {
    //     pimpl_ = make_unique<Model<ModelT, StrategyT>>(std::forward<ModelT>(model)), std::forward<StrategyT>(st); 
    // }

    void draw() const
    {
        pimpl_->draw(); 
    }
    //create copy constructor , copy assignment and others 

    ModelHolder(const ModelHolder& other): pimpl_(other.pimpl_->clone())
    {

    }
    ModelHolder& operator=(ModelHolder other) //other copy constructed
    {
        //ModelHolder tmp{other}; //copy constructor 
        std::swap(other.pimpl_, pimpl_); 
        return *this; 
    }
    ModelHolder(ModelHolder&& other) = default;
    ModelHolder& operator=(ModelHolder&& other) = default;
    ~ModelHolder() = default;

    private: 

    unique_ptr<Concept> pimpl_; 


}; 