#include "core.hpp"
#include <iostream>
#include <regex>
#include <vector>
#include <bitset>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "usage: prime_factoring_cnf number." << std::endl;
        return 1;
    }
    std::string target_str = argv[1];
    if (!std::regex_match(target_str, std::regex("^\\d+$"))) {
        std::cout << "usage: prime_factoring_cnf number." << std::endl;
        return 1;
    }
    int target = std::stoi(target_str);
    int len = 0;
    {
        int t = target;
        while (t > 0) { ++len; t >>= 1; }
    }
    
    std::cout << "Target: " << target << " (bit width: " << len << ")" << std::endl;
    
    std::vector<std::string> conditions;
    
    // Mul_NBit: factor1 * factor2 = target
    {
        Mul_NBit mul_nbit("factor1", "factor2", "target", "overflow", len);
        auto mul_clauses = mul_nbit.expand();
        conditions.insert(conditions.end(), mul_clauses.begin(), mul_clauses.end());
    }
    
    // Input_Not_Equals_Number: factor1 != target
    {
        Input_Not_Equals_Number factor1_not_target("factor1", target, len);
        conditions.push_back(factor1_not_target.expand());
    }
    
    // Input_Not_Equals_Number: factor2 != target
    {
        Input_Not_Equals_Number factor2_not_target("factor2", target, len);
        conditions.push_back(factor2_not_target.expand());
    }
    
    {
        Input_Equals_Number target_constraint("target", target, len);
        auto target_clauses = target_constraint.expand();
        conditions.insert(conditions.end(), target_clauses.begin(), target_clauses.end());
    }
    
    conditions.push_back("-<overflow> 0 ");
    
    {
        Input_Equals_Number one_constraint("One_NBit_" + Z(len), 1, len);
        auto one_clauses = one_constraint.expand();
        conditions.insert(conditions.end(), one_clauses.begin(), one_clauses.end());
    }
    
    {
        Input_Equals_Number one_constraint_2x("One_NBit_" + Z(len*2), 1, len*2);
        auto one_clauses_2x = one_constraint_2x.expand();
        conditions.insert(conditions.end(), one_clauses_2x.begin(), one_clauses_2x.end());
    }
    
    conditions.push_back(" -<Zero_1Bit_" + Z(1) + "> 0 ");
    
    std::string filename = "prime_factoring_" + std::to_string(target) + ".cnf";
    generate_cnf(conditions, filename);
    
    std::cout << "CNF file generated: " << filename << std::endl;
    std::cout << "Looking for factors of: " << target << std::endl;
    std::cout << "This CNF will be satisfiable if " << target << " has non-trivial factors." << std::endl;
    
    return 0;
} 