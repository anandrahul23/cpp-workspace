
// template<typename T> class Function; 



// template<typename R, typename... Args> 
// class Function<R(Args...)> 
// {
//     public: 
//     template<typename Fn> Function(Fn fn) :  impl(std::make_unique<Model<Fn>>(fn)) {}
//     ~Function() = default;
//     Function(Function const& other) : impl(other.impl->clone()) {} // Prototype Design pattern
//     Function& operator=(Function const& other) {
//         //Swap and copy idiom
//         Function tmp(other);
//         std::swap(impl, tmp.impl);
//         return *this;
//     }
//     Function& operator=(Function&&) = default;
//     Function(Function&&) = default;


//     private:
//     struct Concept {
//         virtual ~Concept() = default;
//         virtual R invoke(Args... args) const = 0;
//         virtual std::unique_ptr<Concept> clone() const = 0;
//     };
//     template<typename Fn> struct Model : Concept {
//         Model(Fn fn) : fn_(fn) {}
//         R invoke(Args... args) const override {
//             return fn_(std::forward<Args>(args)...);
//         }
//         std::unique_ptr<Concept> clone() const override {
//            return std::make_unique<Model<Fn>>(fn_);
//         }
//         Fn fn_;
//     };
//     std::unique_ptr<Concept> impl;

// };

