#include "core.hpp"
#include <iostream>
#include <regex>
#include <vector>
#include <string>
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "usage: prime_and_composite_tautology number." << std::endl;
        return 1;
    }
    std::string bit_width_str = argv[1];
    if (!std::regex_match(bit_width_str, std::regex("^\\d+$"))) {
        std::cout << "usage: prime_and_composite_tautology number." << std::endl;
        return 1;
    }
    int bit_width = std::stoi(bit_width_str);
    std::vector<std::string> conditions;
    {
        IsPrime is_prime("target", bit_width, bit_width);
        auto v = is_prime.expand();
        conditions.insert(conditions.end(), v.begin(), v.end());
    }
    {
        IsComposite is_composite("target", bit_width);
        auto v = is_composite.expand();
        conditions.insert(conditions.end(), v.begin(), v.end());
    }
    {
        Input_Equals_Number ien1("One_NBit_" + Z(bit_width), 1, bit_width);
        auto v = ien1.expand();
        conditions.insert(conditions.end(), v.begin(), v.end());
    }
    {
        Input_Equals_Number ien2("One_NBit_" + Z(bit_width*2), 1, bit_width*2);
        auto v = ien2.expand();
        conditions.insert(conditions.end(), v.begin(), v.end());
    }
    conditions.push_back(" -<Zero_1Bit_" + Z(1) + "> 0 ");
    generate_cnf(conditions, "prime_and_composite_tautology_" + std::to_string(bit_width) + ".cnf");
    return 0;
} 