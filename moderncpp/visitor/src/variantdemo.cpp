// #include <iostream>
// #include <variant>
// #include <string>

// int main()
// {
//     // Define a variant that can hold either an int, a float, or a std::string
//     std::variant<int, float, std::string> myVariant;

//     // Assign an int value to the variant
//     myVariant = 10;
//     std::cout << "Integer: " << std::get<int>(myVariant) << std::endl;

//     // Assign a float value to the variant
//     myVariant = 3.14f;
//     std::cout << "Float: " << std::get<float>(myVariant) << std::endl;

//     // Assign a string value to the variant
//     myVariant = std::string("Hello, std::variant!");
//     std::cout << "String: " << std::get<std::string>(myVariant) << std::endl;

//     // Using std::visit to handle the variant
//     std::visit([](auto &&arg)
//                { std::cout << "Visited value: " << arg << std::endl; }, myVariant);

//     return 0;
// }