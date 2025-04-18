#include <iostream>
#include <memory>

using namespace std;

template <typename T, typename... Ts>
unique_ptr<T> make_unique_version1(Ts &&...args)
{
    return unique_ptr<T>{new T(std::forward<Ts>(args)...)};
}

struct A
{

    int i, j, k;
    A(int ii, int jj, int kk) : i{ii}, j{jj}, k{kk}
    {
        cout << "\nConstructed with values:" << i << "," << j << "," << k << endl;
    }
};

int main()
{
    int x = 10;
    int y = 25;
    int z = x + y;

    cout << "Sum of x+y = " << z;

    unique_ptr<A> aPrt = make_unique_version1<A>(10, 2, 3);
}