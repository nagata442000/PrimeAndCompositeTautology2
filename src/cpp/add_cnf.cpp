#include "core.hpp"
#include <iostream>
#include <regex>
#include <vector>
#include <bitset>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "usage: add_cnf number1 number2." << std::endl;
        return 1;
    }
    
    std::string num1_str = argv[1];
    std::string num2_str = argv[2];
    
    if (!std::regex_match(num1_str, std::regex("^\\d+$")) || 
        !std::regex_match(num2_str, std::regex("^\\d+$"))) {
        std::cout << "usage: add_cnf number1 number2." << std::endl;
        return 1;
    }
    
    int num1 = std::stoi(num1_str);
    int num2 = std::stoi(num2_str);
    int sum = num1 + num2;
    
    int max_value = std::max(num1, num2);
    int len = 0;
    {
        int t = max_value;
        while (t > 0) { ++len; t >>= 1; }
    }
    
    int result_len = 0;
    {
        int t = sum;
        while (t > 0) { ++result_len; t >>= 1; }
    }
    
    int final_len = std::max(len + 1, result_len);
    
    std::cout << "Input 1: " << num1 << " (bit width: " << len << ")" << std::endl;
    std::cout << "Input 2: " << num2 << " (bit width: " << len << ")" << std::endl;
    std::cout << "Expected sum: " << sum << " (bit width: " << result_len << ")" << std::endl;
    std::cout << "Using bit width: " << final_len << std::endl;
    
    std::vector<std::string> conditions;

    // Add_NBit.new("input1", "input2", "result", "overflow", final_len)
    {
        Add_NBit add_nbit("input1", "input2", "result", "overflow", final_len);
        auto add_clauses = add_nbit.expand();
        conditions.insert(conditions.end(), add_clauses.begin(), add_clauses.end());
    }
    
    // Input_Equals_Number.new("input1", num1, final_len)
    {
        Input_Equals_Number input1_constraint("input1", num1, final_len);
        auto input1_clauses = input1_constraint.expand();
        conditions.insert(conditions.end(), input1_clauses.begin(), input1_clauses.end());
    }
    
    // Input_Equals_Number.new("input2", num2, final_len)
    {
        Input_Equals_Number input2_constraint("input2", num2, final_len);
        auto input2_clauses = input2_constraint.expand();
        conditions.insert(conditions.end(), input2_clauses.begin(), input2_clauses.end());
    }
    
    conditions.push_back("-<overflow> 0 ");
    
    {
        Input_Equals_Number one_constraint("One_NBit_" + Z(final_len), 1, final_len);
        auto one_clauses = one_constraint.expand();
        conditions.insert(conditions.end(), one_clauses.begin(), one_clauses.end());
    }
    
    conditions.push_back(" -<Zero_1Bit_" + Z(1) + "> 0 ");
    
    std::string filename = "add_" + std::to_string(num1) + "_" + std::to_string(num2) + ".cnf";
    generate_cnf(conditions, filename);
    
    std::cout << "CNF file generated: " << filename << std::endl;
    std::cout << "Expected result: " << sum << std::endl;
    
    return 0;
} 