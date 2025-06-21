#include "core.hpp"
#include <iostream>
#include <regex>
#include <vector>
#include <bitset>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "usage: is_prime number." << std::endl;
        return 1;
    }
    
    std::string target_str = argv[1];
    
    if (!std::regex_match(target_str, std::regex("^\\d+$"))) {
        std::cout << "usage: is_prime number." << std::endl;
        return 1;
    }
    
    int target = std::stoi(target_str);
    
    int len = 0;
    {
        int t = target;
        while (t > 0) { ++len; t >>= 1; }
    }
    
    if (len < 2) len = 2;
    
    std::cout << "Target: " << target << " (bit width: " << len << ")" << std::endl;
    
    std::vector<std::string> conditions;
    
    {
        IsPrime is_prime_op("target", len, len);
        auto is_prime_clauses = is_prime_op.expand();
        conditions.insert(conditions.end(), is_prime_clauses.begin(), is_prime_clauses.end());
    }
    
    {
        Input_Equals_Number target_constraint("target", target, len);
        auto target_clauses = target_constraint.expand();
        conditions.insert(conditions.end(), target_clauses.begin(), target_clauses.end());
    }
    
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
    
    std::string filename = "is_prime_" + std::to_string(target) + ".cnf";
    generate_cnf(conditions, filename);
    
    std::cout << "CNF file generated: " << filename << std::endl;
    std::cout << "Testing if " << target << " is prime." << std::endl;
    std::cout << "This CNF will be satisfiable if " << target << " is prime." << std::endl;
    std::cout << "If the CNF is unsatisfiable, " << target << " is composite." << std::endl;
    
    return 0;
} 