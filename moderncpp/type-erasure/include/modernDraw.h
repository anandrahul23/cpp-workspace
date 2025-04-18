#include <iostream>
#include <vector>
#include <memory>
#include <string>

using namespace std;

class Shape
{

public:
    template <typename ModelT, typename DS>
    Shape(ModelT const &model, DS drawer) : pimpl(make_unique<Model<ModelT, DS>>(model, drawer)) {}

    using Shapes = vector<Shape>;
    // This is where the magic happens

    void draw() const
    {
        pimpl->draw();
    }

private:
    class Concept
    {
    public:
        virtual ~Concept() = default;
        virtual void draw() const = 0;
        virtual string getName() const = 0;
    };
    template <typename ModelT, typename DS>
    class Model : public Concept
    {
    public:
        Model(ModelT const &model, DS const &ds) : model_{model}, drawer_{ds} {}
        void draw() const override
        {

            drawer_(model_);
        }
        string getName() const override
        {
            return model_.getName();
        }

    private:
        ModelT model_;
        DS drawer_;
    };
    unique_ptr<Concept> pimpl;
};