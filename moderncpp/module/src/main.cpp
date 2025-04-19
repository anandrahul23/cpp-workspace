

import leonrahul.CustomStringArray;
#include <iostream>

int main()
{
    StringWorld::CustomStringArray stringArray;

    for (int i = 0; i < 10; ++i)
    {
        stringArray.add(("String " + std::to_string(i + 1)).c_str());
    }

    std::cout << "Strings in the array:" << std::endl;
    for (int i = 0; i < stringArray.getSize(); ++i)
    {
        std::cout << stringArray.get(i) << std::endl;
    }

    return 0;
}
