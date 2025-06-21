#include "core.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <set>
#include <map>
#include <functional>

/**
 * Prime and Composite Number CNF Generator
 * 
 * This file contains a comprehensive C++ implementation for generating Conjunctive Normal Form (CNF)
 * clauses that encode mathematical operations and primality testing. The system is designed to:
 * 
 * 1. Generate CNF clauses for basic arithmetic operations (addition, multiplication, division)
 * 2. Implement logical operations (AND, OR, XOR) in CNF form
 * 3. Create comparison operations (equals, less than) as CNF constraints
 * 4. Encode modular arithmetic operations (power, modular exponentiation)
 * 5. Generate comprehensive primality testing using Fermat tests and mathematical constraints
 * 6. Output CNF files that can be solved by SAT solvers to find prime/composite numbers
 * 
 * The system uses a modular approach where each mathematical operation is represented as a class
 * that can expand itself into CNF clauses. This allows for complex mathematical relationships
 * to be encoded as satisfiability problems.
 */

/**
 * Utility function to format integers with leading zeros
 * Creates a 10-digit zero-padded string representation of an integer
 * Used for generating unique variable names in CNF clauses
 * 
 * @param i The integer to format
 * @return String representation with 10-digit zero-padded format (e.g., "0000000001")
 */
std::string Z(int i) {
    std::ostringstream oss;
    oss << std::setw(10) << std::setfill('0') << i;
    return oss.str();
}

/**
 * Class to represent the condition: input == value
 * Generates CNF clauses that enforce input to be equal to a specific value
 * This is used to set constant values in the mathematical constraints
 * 
 * For each bit position, generates a literal that is true if the bit matches
 * the corresponding bit in the target value, false otherwise
 */
Input_Equals_Number::Input_Equals_Number(const std::string& input, int value, int n) 
    : input(input), value(value), n(n) {}

std::vector<std::string> Input_Equals_Number::expand() const {
    std::vector<std::string> result;
    for (int i = 0; i < n; ++i) {
        if (((value >> i) & 1) == 1) {
            result.push_back("<" + input + "_" + Z(i) + "> 0 ");
        } else {
            result.push_back("-<" + input + "_" + Z(i) + "> 0 ");
        }
    }
    return result;
}

/**
 * Class to represent the condition: input != value
 * Generates CNF clauses that enforce input to be different from a specific value
 * This is used to exclude specific values from the solution space
 * 
 * Creates a single clause that is satisfied when at least one bit differs
 * from the corresponding bit in the target value
 */
Input_Not_Equals_Number::Input_Not_Equals_Number(const std::string& input, int value, int n) 
    : input(input), value(value), n(n) {}

std::string Input_Not_Equals_Number::expand() const {
    std::ostringstream result;
    for (int i = 0; i < n; ++i) {
        if (((value >> i) & 1) == 1) {
            result << "-<" << input << "_" << Z(i) << "> ";
        } else {
            result << "<" << input << "_" << Z(i) << "> ";
        }
    }
    result << " 0 ";
    return result.str();
}

/**
 * Class to represent carry-out logic for 1-bit addition
 * carry_out == (popcount(in_a, in_b, carry_in) >= 2)
 * This implements the carry generation logic for binary addition
 * 
 * The carry-out is 1 when at least 2 of the 3 inputs (a, b, carry_in) are 1
 * This is equivalent to checking if the population count (number of 1s) is >= 2
 */
CarryOut_Equal_POPCNT_GREATER_THAN_2::CarryOut_Equal_POPCNT_GREATER_THAN_2(
    const std::string& in_a, const std::string& in_b, 
    const std::string& carry_in, const std::string& carry_out)
    : in_a(in_a), in_b(in_b), carry_in(carry_in), carry_out(carry_out) {}

std::vector<std::string> CarryOut_Equal_POPCNT_GREATER_THAN_2::expand() const {
    std::vector<std::string> result;
    result.push_back("-<" + in_a + "> -<" + in_b + "> -<" + carry_in + ">  <" + carry_out + "> 0 ");
    result.push_back("-<" + in_a + "> -<" + in_b + ">  <" + carry_in + ">  <" + carry_out + "> 0 ");
    result.push_back("-<" + in_a + ">  <" + in_b + "> -<" + carry_in + ">  <" + carry_out + "> 0 ");
    result.push_back("-<" + in_a + ">  <" + in_b + ">  <" + carry_in + "> -<" + carry_out + "> 0 ");
    result.push_back(" <" + in_a + "> -<" + in_b + "> -<" + carry_in + ">  <" + carry_out + "> 0 ");
    result.push_back(" <" + in_a + "> -<" + in_b + ">  <" + carry_in + "> -<" + carry_out + "> 0 ");
    result.push_back(" <" + in_a + ">  <" + in_b + "> -<" + carry_in + "> -<" + carry_out + "> 0 ");
    result.push_back(" <" + in_a + ">  <" + in_b + ">  <" + carry_in + "> -<" + carry_out + "> 0 ");
    return result;
}

/**
 * Class to represent result logic for 1-bit addition
 * result == in_a XOR in_b XOR carry_in
 * This implements the sum generation logic for binary addition
 * 
 * The result is 1 when an odd number of inputs (a, b, carry_in) are 1
 * This is equivalent to the XOR of all three inputs
 */
Result_Equal_A_XOR_B_XOR_CarryIn::Result_Equal_A_XOR_B_XOR_CarryIn(
    const std::string& in_a, const std::string& in_b, 
    const std::string& carry_in, const std::string& result)
    : in_a(in_a), in_b(in_b), carry_in(carry_in), result(result) {}

std::vector<std::string> Result_Equal_A_XOR_B_XOR_CarryIn::expand() const {
    std::vector<std::string> cnf_clauses;
    cnf_clauses.push_back("-<" + in_a + "> -<" + in_b + "> -<" + carry_in + ">  <" + result + "> 0 ");
    cnf_clauses.push_back("-<" + in_a + "> -<" + in_b + ">  <" + carry_in + "> -<" + result + "> 0 ");
    cnf_clauses.push_back("-<" + in_a + ">  <" + in_b + "> -<" + carry_in + "> -<" + result + "> 0 ");
    cnf_clauses.push_back("-<" + in_a + ">  <" + in_b + ">  <" + carry_in + ">  <" + result + "> 0 ");
    cnf_clauses.push_back(" <" + in_a + "> -<" + in_b + "> -<" + carry_in + "> -<" + result + "> 0 ");
    cnf_clauses.push_back(" <" + in_a + "> -<" + in_b + ">  <" + carry_in + ">  <" + result + "> 0 ");
    cnf_clauses.push_back(" <" + in_a + ">  <" + in_b + "> -<" + carry_in + ">  <" + result + "> 0 ");
    cnf_clauses.push_back(" <" + in_a + ">  <" + in_b + ">  <" + carry_in + "> -<" + result + "> 0 ");
    return cnf_clauses;
}

/**
 * Class to represent 1-bit addition: in_a + in_b + carry_in == (result, carry_out)
 * Combines carry generation and sum generation logic into a complete 1-bit adder
 * 
 * This is the fundamental building block for multi-bit addition operations
 * It generates all necessary CNF clauses to ensure correct addition behavior
 */
Add_1Bit::Add_1Bit(const std::string& in_a, const std::string& in_b, 
                   const std::string& carry_in, const std::string& result, const std::string& carry_out)
    : in_a(in_a), in_b(in_b), carry_in(carry_in), result(result), carry_out(carry_out) {}

std::vector<std::string> Add_1Bit::expand() const {
    std::vector<std::string> result_clauses;
    
    // Generate carry-out constraints
    CarryOut_Equal_POPCNT_GREATER_THAN_2 carry_out_constraint(in_a, in_b, carry_in, carry_out);
    auto carry_out_clauses = carry_out_constraint.expand();
    result_clauses.insert(result_clauses.end(), carry_out_clauses.begin(), carry_out_clauses.end());

    // Generate result constraints
    Result_Equal_A_XOR_B_XOR_CarryIn result_constraint(in_a, in_b, carry_in, result);
    auto result_xor_clauses = result_constraint.expand();
    result_clauses.insert(result_clauses.end(), result_xor_clauses.begin(), result_xor_clauses.end());
    
    return result_clauses;
}

// Static member variable for tracking call counts
int Add_NBit::call_count = 0;

/**
 * Class to represent N-bit addition: in_a + in_b == result
 * Implements ripple-carry addition using multiple 1-bit adders
 * 
 * This creates a chain of 1-bit adders where the carry-out of each stage
 * becomes the carry-in of the next stage, implementing standard binary addition
 */
Add_NBit::Add_NBit(const std::string& in_a, const std::string& in_b, 
                   const std::string& result, const std::string& over_flow, int n)
    : in_a(in_a), in_b(in_b), result(result), over_flow(over_flow), n(n) {}

std::vector<std::string> Add_NBit::expand() const {
    std::vector<std::string> result_clauses;
    
    ++call_count;
    
    // Initialize carry-in to 0 for the first bit
    result_clauses.push_back("-<AddNBit_" + Z(call_count) + "_carry_out_" + Z(0) + "> 0 ");
    
    // Chain 1-bit adders for each bit position
    for (int i = 0; i < n; ++i) {
        Add_1Bit add_1bit(
            in_a + "_" + Z(i),
            in_b + "_" + Z(i),
            "AddNBit_" + Z(call_count) + "_carry_out_" + Z(i),
            result + "_" + Z(i),
            "AddNBit_" + Z(call_count) + "_carry_out_" + Z(i + 1)
        );
        auto add_clauses = add_1bit.expand();
        result_clauses.insert(result_clauses.end(), add_clauses.begin(), add_clauses.end());
    }
    
    // Connect overflow to the final carry-out
    result_clauses.push_back("-<" + over_flow + ">  <AddNBit_" + Z(call_count) + "_carry_out_" + Z(n) + "> 0 ");
    result_clauses.push_back(" <" + over_flow + "> -<AddNBit_" + Z(call_count) + "_carry_out_" + Z(n) + "> 0 ");
    
    return result_clauses;
}

// Static member variable for tracking call counts
int Mul_NBit_1Bit_Shift::call_count = 0;

/**
 * Class to represent multiplication with shift: (in_a * in_b) << shift == result
 * Implements multiplication of N-bit number by 1-bit with left shift
 * 
 * This is a building block for full multiplication, implementing the partial product
 * generation step where each bit of the multiplier is ANDed with the multiplicand
 * and shifted to the appropriate position
 */
Mul_NBit_1Bit_Shift::Mul_NBit_1Bit_Shift(const std::string& in_a, const std::string& in_b, 
                                         const std::string& result, int shift, int n)
    : in_a(in_a), in_b(in_b), result(result), shift(shift), n(n) {}

std::vector<std::string> Mul_NBit_1Bit_Shift::expand() const {
    std::vector<std::string> result_clauses;
    
    // Set lower bits to 0 (shift effect)
    for (int i = 0; i < shift; ++i) {
        result_clauses.push_back("-<" + result + "_" + Z(i) + "> 0 ");
    }
    
    // For each bit position, implement AND logic with shift
    for (int i = 0; i < n; ++i) {
        // result[i+shift] = in_a[i] AND in_b
        result_clauses.push_back(" <" + result + "_" + Z(i + shift) + "> -<" + in_a + "_" + Z(i) + "> -<" + in_b + "> 0 ");
        result_clauses.push_back("-<" + result + "_" + Z(i + shift) + "> -<" + in_a + "_" + Z(i) + ">  <" + in_b + "> 0 ");
        result_clauses.push_back("-<" + result + "_" + Z(i + shift) + ">  <" + in_a + "_" + Z(i) + "> -<" + in_b + "> 0 ");
        result_clauses.push_back("-<" + result + "_" + Z(i + shift) + ">  <" + in_a + "_" + Z(i) + ">  <" + in_b + "> 0 ");
    }
    
    // Set upper bits beyond result range to 0
    for (int i = shift + n; i < n * 2; ++i) {
        result_clauses.push_back("-<" + result + "_" + Z(i) + "> 0 ");
    }
    
    return result_clauses;
}

// Static member variable for tracking call counts
int Mul_NBit::call_count = 0;

/**
 * Class to represent N-bit multiplication: in_a * in_b == result
 * Implements multiplication using shift-and-add algorithm
 */
Mul_NBit::Mul_NBit(const std::string& in_a, const std::string& in_b, 
                   const std::string& result, const std::string& over_flow, int n)
    : in_a(in_a), in_b(in_b), result(result), over_flow(over_flow), n(n) {}

std::vector<std::string> Mul_NBit::expand() const {
    std::vector<std::string> result_clauses;
    
    ++call_count;
    
    // Generate partial products for each bit of in_b
    for (int i = 0; i < n; ++i) {
        Mul_NBit_1Bit_Shift mul_shift(
            in_a,
            in_b + "_" + Z(i),
            "Mul_NBit_Accum1_" + Z(call_count) + "_" + Z(i),
            i,
            n
        );
        auto mul_clauses = mul_shift.expand();
        result_clauses.insert(result_clauses.end(), mul_clauses.begin(), mul_clauses.end());
    }

    // Initialize accumulator to 0
    for (int i = 0; i < n * 2; ++i) {
        result_clauses.push_back("-<Mul_NBit_Accum2_" + Z(call_count) + "_" + Z(0) + "_" + Z(i) + "> 0 ");
    }
    
    // Add partial products to accumulator
    for (int i = 0; i < n; ++i) {
        Add_NBit add_nbit(
            "Mul_NBit_Accum1_" + Z(call_count) + "_" + Z(i),
            "Mul_NBit_Accum2_" + Z(call_count) + "_" + Z(i),
            "Mul_NBit_Accum2_" + Z(call_count) + "_" + Z(i + 1),
            "Mul_NBit_CarryOut_" + Z(call_count) + "_" + Z(i),
            n * 2
        );
        auto add_clauses = add_nbit.expand();
        result_clauses.insert(result_clauses.end(), add_clauses.begin(), add_clauses.end());
    }
    
    // Connect result to final accumulator value
    for (int i = 0; i < n; ++i) {
        result_clauses.push_back("-<" + result + "_" + Z(i) + ">  <Mul_NBit_Accum2_" + Z(call_count) + "_" + Z(n) + "_" + Z(i) + "> 0 ");
        result_clauses.push_back(" <" + result + "_" + Z(i) + "> -<Mul_NBit_Accum2_" + Z(call_count) + "_" + Z(n) + "_" + Z(i) + "> 0 ");
    }
    
    // Generate overflow condition: if any upper bits are set, overflow occurs
    std::ostringstream overflow_clause;
    overflow_clause << "-<" << over_flow << "> ";
    for (int i = 0; i < n; ++i) {
        overflow_clause << " <Mul_NBit_Accum2_" << Z(call_count) << "_" << Z(n) << "_" << Z(i + n) << "> ";
    }
    overflow_clause << " 0 ";
    result_clauses.push_back(overflow_clause.str());
    
    // If overflow is set, at least one upper bit must be set
    for (int i = 0; i < n; ++i) {
        result_clauses.push_back("<" + over_flow + ">  -<Mul_NBit_Accum2_" + Z(call_count) + "_" + Z(n) + "_" + Z(i + n) + "> 0 ");
    }
    
    return result_clauses;
}

class ExpandableCondition {
public:
    virtual ~ExpandableCondition() = default;
    virtual std::vector<std::string> expand() const = 0;
};


class StringCondition : public ExpandableCondition {
private:
    std::string condition;
public:
    StringCondition(const std::string& cond) : condition(cond) {}
    std::vector<std::string> expand() const override {
        return {condition};
    }
};

void generate_cnf(const std::vector<std::string>& conditions, const std::string& file_path) {
    std::vector<std::string> expanded_conditions = conditions;
    
    bool changed = true;
    int iter = 0;
    while (changed) {
        iter++;
        changed = false;
        std::cerr << "expand : " << iter << std::endl;
        
        std::vector<std::string> new_conditions;
        for (const auto& condition : expanded_conditions) {
            new_conditions.push_back(condition);
        }
        expanded_conditions = new_conditions;
    }
    
    std::cerr << "gather literals..." << std::endl;
    std::set<std::string> literals_set;
    
    std::regex literal_regex(R"(<[a-zA-Z0-9_]+>)");
    for (size_t i = 0; i < expanded_conditions.size(); ++i) {
        if ((i % (expanded_conditions.size() / 20)) == 0) {
            std::cerr << (5 * i / (expanded_conditions.size() / 20)) << "%..." << std::endl;
        }
        
        std::string clause = expanded_conditions[i];
        std::sregex_iterator it(clause.begin(), clause.end(), literal_regex);
        std::sregex_iterator end;
        for (; it != end; ++it) {
            literals_set.insert(it->str());
        }
    }
    
    std::cerr << "sorting literals..." << std::endl;
    std::vector<std::string> literals(literals_set.begin(), literals_set.end());
    
    std::sort(literals.begin(), literals.end(), [](const std::string& a, const std::string& b) {
        bool a_upper = (a[1] >= 'A' && a[1] <= 'Z');
        bool b_upper = (b[1] >= 'A' && b[1] <= 'Z');
        
        if (a_upper && b_upper) {
            return a < b;
        } else if (!a_upper && !b_upper) {
            return a < b;
        } else if (a_upper && !b_upper) {
            return false;
        } else {
            return true;
        }
    });
    
    std::cerr << "mapping symbol to integer..." << std::endl;
    std::map<std::string, int> literal_map;
    for (size_t i = 0; i < literals.size(); ++i) {
        literal_map[literals[i]] = i + 1;
    }
    
    std::cerr << "replacing symbol to integer..." << std::endl;
    std::vector<std::string> replaced;
    for (size_t iter = 0; iter < expanded_conditions.size(); ++iter) {
        if ((iter % (expanded_conditions.size() / 20)) == 0) {
            std::cerr << (5 * iter / (expanded_conditions.size() / 20)) << "%..." << std::endl;
        }
        
        std::string clause = expanded_conditions[iter];
        
        // Extract literals that appear in this clause only
        std::sregex_iterator it(clause.begin(), clause.end(), literal_regex);
        std::sregex_iterator end;
        std::set<std::string> clause_literals;
        for (; it != end; ++it) {
            clause_literals.insert(it->str());
        }
        
        // Replace only the literals that appear in this clause
        for (const auto& literal : clause_literals) {
            size_t pos = 0;
            while ((pos = clause.find(literal, pos)) != std::string::npos) {
                clause.replace(pos, literal.length(), std::to_string(literal_map[literal]));
                pos += std::to_string(literal_map[literal]).length();
            }
        }
        replaced.push_back(clause);
    }
    
    std::cerr << "writing cnf to file..." << std::endl;
    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << " for writing" << std::endl;
        return;
    }
    
    file << "c\n";
    file << "c\n";
    file << "c\n";
    
    for (const auto& [literal, value] : literal_map) {
        file << "cv " << literal << " " << value << "\n";
    }
    
    file << "p cnf " << literal_map.size() << " " << replaced.size() << "\n";
    
    for (size_t i = 0; i < replaced.size(); ++i) {
        if ((i % (replaced.size() / 20)) == 0) {
            std::cerr << (5 * i / (replaced.size() / 20)) << "%..." << std::endl;
        }
        file << replaced[i] << "\n";
    }
    
    file.close();
    std::cerr << "CNF file generated successfully: " << file_path << std::endl;
}

// Static member variable definition for IsPrime
int IsPrime::call_count = 0;

/**
 * Class to represent primality testing: target is a prime number
 * Implements comprehensive primality testing using multiple Fermat tests and mathematical constraints
 */
IsPrime::IsPrime(const std::string& target, int n, int num_prime)
    : target(target), n(n), num_prime(num_prime == -1 ? n : num_prime) {}

std::vector<std::string> IsPrime::expand() const {
//    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Input_Not_Equals_Number for prime[i] != 0
    for (int i = 0; i < num_prime; i++) {
        auto not_zero_clause = Input_Not_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 0, n).expand();
        clauses.push_back(not_zero_clause);
    }
    
    // Input_Not_Equals_Number for prime[i] != 1
    for (int i = 0; i < num_prime; i++) {
        auto not_one_clause = Input_Not_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 1, n).expand();
        clauses.push_back(not_one_clause);
    }
    
    // Pow_NBit for pow_temp[i][j] = pow(prime[j], pow[i][j])
    for (int i = 0; i < num_prime; i++) {
        for (int j = 0; j < num_prime; j++) {
            Pow_NBit pow_op("IsPrime_Prime_" + Z(call_count) + "_" + Z(j),
                           "IsPrime_Pow_" + Z(call_count) + "_" + Z(i) + "_" + Z(j),
                           "IsPrime_PowTemp_" + Z(call_count) + "_" + Z(i) + "_" + Z(j),
                           "IsPrime_PowTemp_Overflow_" + Z(call_count) + "_" + Z(i) + "_" + Z(j),
                           n);
            auto pow_clauses = pow_op.expand();
            clauses.insert(clauses.end(), pow_clauses.begin(), pow_clauses.end());
        }
    }
    
    // powtemp_overflow = 0
    for (int i = 0; i < num_prime; i++) {
        for (int j = 0; j < num_prime; j++) {
            clauses.push_back("-<IsPrime_PowTemp_Overflow_" + Z(call_count) + "_" + Z(i) + "_" + Z(j) + "> 0 ");
        }
    }
    
    // Product_NBit for product[i] = product j (pow_temp[i][j])
    for (int i = 0; i < num_prime; i++) {
        Product_NBit product_op("IsPrime_PowTemp_" + Z(call_count) + "_" + Z(i),
                               "IsPrime_Product_" + Z(call_count) + "_" + Z(i),
                               "IsPrime_Product_Overflow_" + Z(call_count) + "_" + Z(i),
                               num_prime,
                               n);
        auto product_clauses = product_op.expand();
        clauses.insert(clauses.end(), product_clauses.begin(), product_clauses.end());
    }
    
    // product_overflow = 0
    for (int i = 0; i < num_prime; i++) {
        clauses.push_back("-<IsPrime_Product_Overflow_" + Z(call_count) + "_" + Z(i) + "> 0 ");
    }
    
    // Add_NBit for product_plus1[i] = product[i] + 1
    for (int i = 0; i < num_prime; i++) {
        Add_NBit add_op("IsPrime_Product_" + Z(call_count) + "_" + Z(i),
                        "One_NBit_" + Z(n),
                        "IsPrime_Product_Plus1_" + Z(call_count) + "_" + Z(i),
                        "IsPrime_Product_Plus1_Overflow_" + Z(call_count) + "_" + Z(i),
                        n);
        auto add_clauses = add_op.expand();
        clauses.insert(clauses.end(), add_clauses.begin(), add_clauses.end());
    }
    
    // product_plus1_overflow[i] = 0
    for (int i = 0; i < num_prime; i++) {
        clauses.push_back("-<IsPrime_Product_Plus1_Overflow_" + Z(call_count) + "_" + Z(i) + "> 0 ");
    }
    
    // Sum_NBit for sumpow[i] = sum j pow[i][j]
    for (int i = 0; i < num_prime; i++) {
        Sum_NBit sum_op("IsPrime_Pow_" + Z(call_count) + "_" + Z(i),
                        "IsPrime_SumPow_" + Z(call_count) + "_" + Z(i),
                        "IsPrime_SumPow_Overflow_" + Z(call_count) + "_" + Z(i),
                        num_prime,
                        n);
        auto sum_clauses = sum_op.expand();
        clauses.insert(clauses.end(), sum_clauses.begin(), sum_clauses.end());
    }
    
    // sumpow_overflow[i] = 0
    for (int i = 0; i < num_prime; i++) {
        clauses.push_back("-<IsPrime_SumPow_Overflow_" + Z(call_count) + "_" + Z(i) + "> 0 ");
    }
    
    // Or_Condition for prime[i] == 2 or (1 < sumpow[i] and product_plus1[i] == prime[i])
    for (int i = 0; i < num_prime; i++) {
        // Create the inner conditions
        std::vector<std::string> prime_equals_2 = Input_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 2, n).expand();
        std::vector<std::string> prime_equals_3 = Input_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 3, n).expand();
        
        // Create the less than and equals conditions
        LessThan_NBit less_than_op("One_NBit_" + Z(n), "IsPrime_SumPow_" + Z(call_count) + "_" + Z(i), n);
        auto less_than_clauses = less_than_op.expand();
        
        Equals_NBit equals_op("IsPrime_Product_Plus1_" + Z(call_count) + "_" + Z(i),
                             "IsPrime_Prime_" + Z(call_count) + "_" + Z(i),
                             n);
        auto equals_clauses = equals_op.expand();
        
        // Combine conditions using Or_Condition and And_Condition
        Or_Condition inner_or(prime_equals_2, prime_equals_3);
        And_Condition inner_and(less_than_clauses, equals_clauses);
        Or_Condition outer_or(inner_or.expand(), inner_and.expand());
        
        auto outer_clauses = outer_or.expand();
        clauses.insert(clauses.end(), outer_clauses.begin(), outer_clauses.end());
    }
    
    // Add_NBit for prime_minus1[i] = prime[i] - 1
    for (int i = 0; i < num_prime; i++) {
        Add_NBit add_op("IsPrime_Prime_Minus1_" + Z(call_count) + "_" + Z(i),
                        "One_NBit_" + Z(n),
                        "IsPrime_Prime_" + Z(call_count) + "_" + Z(i),
                        "IsPrime_Prime_Minus1_Overflow_" + Z(call_count) + "_" + Z(i),
                        n);
        auto add_clauses = add_op.expand();
        clauses.insert(clauses.end(), add_clauses.begin(), add_clauses.end());
    }
    
    // prime_minus1_overflow[i] = 0
    for (int i = 0; i < num_prime; i++) {
        clauses.push_back("-<IsPrime_Prime_Minus1_Overflow_" + Z(call_count) + "_" + Z(i) + "> 0 ");
    }
    
    // DivMod_NBit for div[i][j] = prime_minus1[i] / prime[j]
    for (int i = 0; i < num_prime; i++) {
        for (int j = 0; j < num_prime; j++) {
            DivMod_NBit divmod_op("IsPrime_Prime_Minus1_" + Z(call_count) + "_" + Z(i),
                                 "IsPrime_Prime_" + Z(call_count) + "_" + Z(j),
                                 "IsPrime_Div_" + Z(call_count) + "_" + Z(i) + "_" + Z(j),
                                 "IsPrime_Mod_" + Z(call_count) + "_" + Z(i) + "_" + Z(j),
                                 n);
            auto divmod_clauses = divmod_op.expand();
            clauses.insert(clauses.end(), divmod_clauses.begin(), divmod_clauses.end());
        }
    }
    
    // Or_Condition for Fermat test conditions
    for (int i = 0; i < num_prime; i++) {
        for (int j = 0; j < num_prime; j++) {
            // Create FermatTest3 condition
            FermatTest3 fermat_op("IsPrime_Generator_" + Z(call_count) + "_" + Z(i),
                                 "IsPrime_Div_" + Z(call_count) + "_" + Z(i) + "_" + Z(j),
                                 "IsPrime_Prime_" + Z(call_count) + "_" + Z(i),
                                 n);
            auto fermat_clauses = fermat_op.expand();
            
            // Create pow[i][j] == 0 condition
            std::vector<std::string> pow_zero = Input_Equals_Number("IsPrime_Pow_" + Z(call_count) + "_" + Z(i) + "_" + Z(j), 0, n).expand();
            
            // Create prime[i] == 2 or prime[i] == 3 condition
            std::vector<std::string> prime_equals_2 = Input_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 2, n).expand();
            std::vector<std::string> prime_equals_3 = Input_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 3, n).expand();
            
            // Combine conditions
            Or_Condition inner_or1(fermat_clauses, pow_zero);
            Or_Condition inner_or2(prime_equals_2, prime_equals_3);
            Or_Condition outer_or(inner_or1.expand(), inner_or2.expand());
            
            auto outer_clauses = outer_or.expand();
            clauses.insert(clauses.end(), outer_clauses.begin(), outer_clauses.end());
        }
    }
    
    // Or_Condition for final Fermat test
    for (int i = 0; i < num_prime; i++) {
        // Create FermatTest2 condition
        FermatTest2 fermat_op("IsPrime_Generator_" + Z(call_count) + "_" + Z(i),
                             "IsPrime_Prime_" + Z(call_count) + "_" + Z(i),
                             n);
        auto fermat_clauses = fermat_op.expand();
        
        // Create prime[i] == 2 or prime[i] == 3 condition
        std::vector<std::string> prime_equals_2 = Input_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 2, n).expand();
        std::vector<std::string> prime_equals_3 = Input_Equals_Number("IsPrime_Prime_" + Z(call_count) + "_" + Z(i), 3, n).expand();
        
        // Combine conditions
        Or_Condition inner_or(prime_equals_2, prime_equals_3);
        Or_Condition outer_or(fermat_clauses, inner_or.expand());
        
        auto outer_clauses = outer_or.expand();
        clauses.insert(clauses.end(), outer_clauses.begin(), outer_clauses.end());
    }
    
    // Equals_NBit for target == prime[0]
    Equals_NBit target_equals_op(target, "IsPrime_Prime_" + Z(call_count) + "_" + Z(0), n);
    auto target_clauses = target_equals_op.expand();
    clauses.insert(clauses.end(), target_clauses.begin(), target_clauses.end());
    
    return clauses;
}

/**
 * Class to represent composite number testing: target is a composite number
 * Implements composite number detection by finding two non-trivial factors
 */
IsComposite::IsComposite(const std::string& target, int n)
    : target(target), n(n) {}

std::vector<std::string> IsComposite::expand() const {
    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Mul_NBit for factor1 * factor2 = target
    auto mul_clauses = Mul_NBit("IsComposite_fact1_" + Z(call_count),
                               "IsComposite_fact2_" + Z(call_count),
                               target,
                               "IsComposite_Overflow_" + Z(call_count),
                               n).expand();
    clauses.insert(clauses.end(), mul_clauses.begin(), mul_clauses.end());
    
    // Input_Not_Equals_Number for factor1 != 0
    auto fact1_not_zero_clause = Input_Not_Equals_Number("IsComposite_fact1_" + Z(call_count), 0, n).expand();
    clauses.push_back(fact1_not_zero_clause);
    
    // Input_Not_Equals_Number for factor2 != 0
    auto fact2_not_zero_clause = Input_Not_Equals_Number("IsComposite_fact2_" + Z(call_count), 0, n).expand();
    clauses.push_back(fact2_not_zero_clause);
    
    // Input_Not_Equals_Number for factor1 != 1
    auto fact1_not_one_clause = Input_Not_Equals_Number("IsComposite_fact1_" + Z(call_count), 1, n).expand();
    clauses.push_back(fact1_not_one_clause);
    
    // Input_Not_Equals_Number for factor2 != 1
    auto fact2_not_one_clause = Input_Not_Equals_Number("IsComposite_fact2_" + Z(call_count), 1, n).expand();
    clauses.push_back(fact2_not_one_clause);
    
    // No overflow
    clauses.push_back("-<IsComposite_Overflow_" + Z(call_count) + "> 0 ");
    
    return clauses;
}

// Static member variable for tracking call counts
int Mul_NBit_1Bit::call_count = 0;

/**
 * Class to represent N-bit multiplication by 1-bit: in_a * in_b == result
 * Implements bitwise AND operation for each bit position
 */
Mul_NBit_1Bit::Mul_NBit_1Bit(const std::string& in_a, const std::string& in_b, 
                             const std::string& result, int n) 
    : in_a(in_a), in_b(in_b), result(result), n(n) {}

std::vector<std::string> Mul_NBit_1Bit::expand() const {
    std::vector<std::string> clauses;
    
    for (int i = 0; i < n; i++) {
        // For each bit position, add clauses that enforce:
        // result[i] = in_a[i] & in_b
        clauses.push_back(" <" + result + "_" + Z(i) + "> -<" + in_a + "_" + Z(i) + "> -<" + in_b + "> 0 ");
        clauses.push_back("-<" + result + "_" + Z(i) + "> -<" + in_a + "_" + Z(i) + ">  <" + in_b + "> 0 ");
        clauses.push_back("-<" + result + "_" + Z(i) + ">  <" + in_a + "_" + Z(i) + "> -<" + in_b + "> 0 ");
        clauses.push_back("-<" + result + "_" + Z(i) + ">  <" + in_a + "_" + Z(i) + ">  <" + in_b + "> 0 ");
    }
    
    return clauses;
}

// Static member variable for tracking call counts
int And_1Bit::call_count = 0;

/**
 * Class to represent 1-bit AND operation: in_a & in_b == result
 * Implements logical AND using CNF clauses
 */
And_1Bit::And_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result)
    : in_a(in_a), in_b(in_b), result(result) {}

std::vector<std::string> And_1Bit::expand() const {
    return {
        " <" + in_a + ">  <" + in_b + "> -<" + result + "> 0 ",
        " <" + in_a + "> -<" + in_b + "> -<" + result + "> 0 ",
        "-<" + in_a + ">  <" + in_b + "> -<" + result + "> 0 ",
        "-<" + in_a + "> -<" + in_b + ">  <" + result + "> 0 "
    };
}

// Static member variable for tracking call counts
int LessThan_1Bit::call_count = 0;

/**
 * Class to represent 1-bit less-than comparison: result == (in_a < in_b)
 * Implements comparison logic using CNF clauses
 */
LessThan_1Bit::LessThan_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result)
    : in_a(in_a), in_b(in_b), result(result) {}

std::vector<std::string> LessThan_1Bit::expand() const {
    return {
        " <" + in_a + ">  <" + in_b + "> -<" + result + "> 0 ",
        " <" + in_a + "> -<" + in_b + ">  <" + result + "> 0 ",
        "-<" + in_a + ">  <" + in_b + "> -<" + result + "> 0 ",
        "-<" + in_a + "> -<" + in_b + "> -<" + result + "> 0 "
    };
}

// Static member variable for tracking call counts
int Equals_1Bit::call_count = 0;

/**
 * Class to represent 1-bit equality comparison: result == (in_a == in_b)
 * Implements equality logic using CNF clauses
 */
Equals_1Bit::Equals_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result)
    : in_a(in_a), in_b(in_b), result(result) {}

std::vector<std::string> Equals_1Bit::expand() const {
    return {
        " <" + in_a + ">  <" + in_b + ">  <" + result + "> 0 ",
        " <" + in_a + "> -<" + in_b + "> -<" + result + "> 0 ",
        "-<" + in_a + ">  <" + in_b + "> -<" + result + "> 0 ",
        "-<" + in_a + "> -<" + in_b + ">  <" + result + "> 0 "
    };
}

// Static member variable for tracking call counts
int Equals_NBit::call_count = 0;

/**
 * Class to represent N-bit equality comparison: in_a == in_b
 * Implements equality for each bit position
 */
Equals_NBit::Equals_NBit(const std::string& in_a, const std::string& in_b, int n)
    : in_a(in_a), in_b(in_b), n(n) {}

std::vector<std::string> Equals_NBit::expand() const {
    std::vector<std::string> clauses;
    for (int i = 0; i < n; i++) {
        clauses.push_back("-<" + in_a + "_" + Z(i) + ">  <" + in_b + "_" + Z(i) + "> 0 ");
        clauses.push_back(" <" + in_a + "_" + Z(i) + "> -<" + in_b + "_" + Z(i) + "> 0 ");
    }
    return clauses;
}

// Static member variable for tracking call counts
int LessThan_NBit::call_count = 0;

/**
 * Class to represent N-bit less-than comparison: in_a < in_b
 * Implements comparison using bit-by-bit analysis with carry logic
 */
LessThan_NBit::LessThan_NBit(const std::string& in_a, const std::string& in_b, int n)
    : in_a(in_a), in_b(in_b), n(n) {}

std::vector<std::string> LessThan_NBit::expand() const {
    std::vector<std::string> clauses;
    call_count++;

    // Generate Equals_1Bit clauses for each bit position
    for (int i = 0; i < n; i++) {
        Equals_1Bit equals(in_a + "_" + Z(i), in_b + "_" + Z(i),
                          "LessThan_NBit_Equals_" + Z(call_count) + "_" + Z(i));
        auto equals_clauses = equals.expand();
        clauses.insert(clauses.end(), equals_clauses.begin(), equals_clauses.end());
    }

    // Generate LessThan_1Bit clauses for each bit position
    for (int i = 0; i < n; i++) {
        LessThan_1Bit less_than(in_a + "_" + Z(i), in_b + "_" + Z(i),
                               "LessThan_NBit_Less_" + Z(call_count) + "_" + Z(i));
        auto less_than_clauses = less_than.expand();
        clauses.insert(clauses.end(), less_than_clauses.begin(), less_than_clauses.end());
    }

    // Add initial equal accumulation clause
    clauses.push_back("<LessThan_NBit_EqualAccum_" + Z(call_count) + "_" + Z(n) + "> 0 ");

    // Generate And_1Bit clauses for equal accumulation
    for (int i = 0; i < n; i++) {
        And_1Bit and_op("LessThan_NBit_EqualAccum_" + Z(call_count) + "_" + Z(i+1),
                       "LessThan_NBit_Equals_" + Z(call_count) + "_" + Z(i),
                       "LessThan_NBit_EqualAccum_" + Z(call_count) + "_" + Z(i));
        auto and_clauses = and_op.expand();
        clauses.insert(clauses.end(), and_clauses.begin(), and_clauses.end());
    }

    // Generate And_1Bit clauses for result
    for (int i = 0; i < n; i++) {
        And_1Bit and_op("LessThan_NBit_EqualAccum_" + Z(call_count) + "_" + Z(i+1),
                       "LessThan_NBit_Less_" + Z(call_count) + "_" + Z(i),
                       "LessThan_NBit_Result_" + Z(call_count) + "_" + Z(i));
        auto and_clauses = and_op.expand();
        clauses.insert(clauses.end(), and_clauses.begin(), and_clauses.end());
    }

    // Add final result clause
    std::string result_clause;
    for (int i = 0; i < n; i++) {
        result_clause += " <LessThan_NBit_Result_" + Z(call_count) + "_" + Z(i) + "> ";
    }
    result_clause += " 0 ";
    clauses.push_back(result_clause);

    return clauses;
}

// Static member variable for tracking call counts
int DivMod_NBit::call_count = 0;

/**
 * Class to represent division and modulo: in_a == in_b * div + mod
 * Implements division using multiplication and addition constraints
 */


DivMod_NBit::DivMod_NBit(const std::string& in_a, const std::string& in_b, 
                         const std::string& div, const std::string& mod, int n)
    : in_a(in_a), in_b(in_b), div(div), mod(mod), n(n) {}

std::vector<std::string> DivMod_NBit::expand() const {
    std::vector<std::string> clauses;
    call_count++;

    // Multiply in_b and div, store in accumulator
    Mul_NBit mul_op(in_b, div, 
                   "DivMod_NBit_Accum_" + Z(call_count),
                   "DivMode_NBit_MulOverflow_" + Z(call_count),
                   n);
    auto mul_clauses = mul_op.expand();
    clauses.insert(clauses.end(), mul_clauses.begin(), mul_clauses.end());

    // Add mod to accumulator, result should equal in_a
    Add_NBit add_op("DivMod_NBit_Accum_" + Z(call_count),
                   mod,
                   in_a,
                   "DivMode_NBit_AddOverflow_" + Z(call_count),
                   n);
    auto add_clauses = add_op.expand();
    clauses.insert(clauses.end(), add_clauses.begin(), add_clauses.end());

    // Ensure no overflow in multiplication
    clauses.push_back("-<DivMode_NBit_MulOverflow_" + Z(call_count) + "> 0 ");

    // Ensure no overflow in addition
    clauses.push_back("-<DivMode_NBit_AddOverflow_" + Z(call_count) + "> 0 ");

    // Ensure mod is less than in_b
    LessThan_NBit less_than(mod, in_b, n);
    auto less_than_clauses = less_than.expand();
    clauses.insert(clauses.end(), less_than_clauses.begin(), less_than_clauses.end());

    return clauses;
}



// Static member variable for tracking call counts
int If_Cond_A_Else_B_1Bit::call_count = 0;

/**
 * Class to represent 1-bit conditional: result == if cond then in_a else in_b
 * Implements multiplexer logic using CNF clauses
 */
If_Cond_A_Else_B_1Bit::If_Cond_A_Else_B_1Bit(const std::string& in_a, const std::string& in_b, 
                                             const std::string& cond, const std::string& result)
    : in_a(in_a), in_b(in_b), cond(cond), result(result) {}

std::vector<std::string> If_Cond_A_Else_B_1Bit::expand() const {
    return {
        "-<" + cond + "> -<" + in_a + ">  <" + result + "> 0 ",
        "-<" + cond + ">  <" + in_a + "> -<" + result + "> 0 ",
        " <" + cond + "> -<" + in_b + ">  <" + result + "> 0 ",
        " <" + cond + ">  <" + in_b + "> -<" + result + "> 0 "
    };
}

// Static member variable for tracking call counts
int If_Cond_A_Else_B_NBit::call_count = 0;

/**
 * Class to represent N-bit conditional: result == if cond then in_a else in_b
 * Implements conditional selection for each bit position
 */
If_Cond_A_Else_B_NBit::If_Cond_A_Else_B_NBit(const std::string& in_a, const std::string& in_b, 
                                             const std::string& cond, const std::string& result, int n)
    : in_a(in_a), in_b(in_b), cond(cond), result(result), n(n) {}

std::vector<std::string> If_Cond_A_Else_B_NBit::expand() const {
    std::vector<std::string> clauses;
    
    for (int i = 0; i < n; i++) {
        If_Cond_A_Else_B_1Bit if_op(in_a + "_" + Z(i),
                                   in_b + "_" + Z(i),
                                   cond,
                                   result + "_" + Z(i));
        auto if_clauses = if_op.expand();
        clauses.insert(clauses.end(), if_clauses.begin(), if_clauses.end());
    }
    
    return clauses;
}

/**
 * Class to represent 1-bit OR operation: result == in_a | in_b
 * Implements logical OR using CNF clauses
 */
Or_1Bit::Or_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result)
    : in_a(in_a), in_b(in_b), result(result) {}

std::vector<std::string> Or_1Bit::expand() const {
    return {
        "-<" + in_a + "> -<" + in_b + ">  <" + result + "> 0 ",
        "-<" + in_a + ">  <" + in_b + ">  <" + result + "> 0 ",
        " <" + in_a + "> -<" + in_b + ">  <" + result + "> 0 ",
        " <" + in_a + ">  <" + in_b + "> -<" + result + "> 0 "
    };
}

/**
 * Class to represent N-bit to 1-bit OR reduction: result == in_a_1 | in_a_2 | ... | in_a_n
 * Implements OR operation across multiple bits to produce a single result
 */
Or_NBit_To_1Bit::Or_NBit_To_1Bit(const std::string& in_a, const std::string& result, int n)
    : in_a(in_a), result(result), n(n) {}

std::vector<std::string> Or_NBit_To_1Bit::expand() const {
    std::vector<std::string> clauses;
    
    // First clause: if result is false, all inputs must be false
    std::string first_clause = "-<" + result + "> ";
    for (int i = 0; i < n; i++) {
        first_clause += " <" + in_a + "_" + Z(i) + "> ";
    }
    first_clause += " 0 ";
    clauses.push_back(first_clause);
    
    // Remaining clauses: if any input is true, result must be true
    for (int i = 0; i < n; i++) {
        clauses.push_back("<" + result + "> -<" + in_a + "_" + Z(i) + "> 0 ");
    }
    
    return clauses;
}

/**
 * Class to represent power operation: result == in_a ** in_b
 * Implements exponentiation using repeated squaring algorithm
 */
Pow_NBit::Pow_NBit(const std::string& in_a, const std::string& in_b, const std::string& result, const std::string& over_flow, int n)
    : in_a(in_a), in_b(in_b), result(result), over_flow(over_flow), n(n) {}

std::vector<std::string> Pow_NBit::expand() const {
    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Equals_NBit for temp1[0] and in_a
    auto equals_clauses = Equals_NBit("Pow_NBit_Temp1_" + Z(call_count) + "_" + Z(0), in_a, n).expand();
    clauses.insert(clauses.end(), equals_clauses.begin(), equals_clauses.end());
    
    // Mul_NBit for temp1[i] * temp1[i] = temp1[i+1] (repeated squaring)
    for (int i = 0; i < n; i++) {
        auto mul_clauses = Mul_NBit("Pow_NBit_Temp1_" + Z(call_count) + "_" + Z(i),
                                  "Pow_NBit_Temp1_" + Z(call_count) + "_" + Z(i),
                                  "Pow_NBit_Temp1_" + Z(call_count) + "_" + Z(i+1),
                                  "Pow_NBit_Temp1Overflow_" + Z(call_count) + "_" + Z(i),
                                  n).expand();
        clauses.insert(clauses.end(), mul_clauses.begin(), mul_clauses.end());
    }
    
    // If_Cond_A_Else_B_NBit for temp2[i] (select power of 2 or 1 based on exponent bit)
    for (int i = 0; i < n; i++) {
        auto if_clauses = If_Cond_A_Else_B_NBit("Pow_NBit_Temp1_" + Z(call_count) + "_" + Z(i),
                                               "One_NBit_" + Z(n),
                                               in_b + "_" + Z(i),
                                               "Pow_NBit_Temp2_" + Z(call_count) + "_" + Z(i),
                                               n).expand();
        clauses.insert(clauses.end(), if_clauses.begin(), if_clauses.end());
    }
    
    // Input_Equals_Number for pow_accum[0] = 1
    auto input_clauses = Input_Equals_Number("Pow_NBit_PowAccum_" + Z(call_count) + "_" + Z(0), 1, n).expand();
    clauses.insert(clauses.end(), input_clauses.begin(), input_clauses.end());
    
    // Mul_NBit for pow_accum[i+1] (accumulate the result)
    for (int i = 0; i < n; i++) {
        auto mul_clauses = Mul_NBit("Pow_NBit_Temp2_" + Z(call_count) + "_" + Z(i),
                                  "Pow_NBit_PowAccum_" + Z(call_count) + "_" + Z(i),
                                  "Pow_NBit_PowAccum_" + Z(call_count) + "_" + Z(i+1),
                                  "Pow_NBit_PowAccumOverflow_" + Z(call_count) + "_" + Z(i),
                                  n).expand();
        clauses.insert(clauses.end(), mul_clauses.begin(), mul_clauses.end());
    }
    
    // Equals_NBit for result and pow_accum[n]
    auto result_clauses = Equals_NBit(result,
                                    "Pow_NBit_PowAccum_" + Z(call_count) + "_" + Z(n),
                                    n).expand();
    clauses.insert(clauses.end(), result_clauses.begin(), result_clauses.end());
    
    // Initialize overflow accum
    clauses.push_back("-<Pow_NBit_PowAccumOverflowAccum_" + Z(call_count) + "_" + Z(0) + "> 0 ");
    
    // Or_1Bit for overflow accum (track overflow across iterations)
    for (int i = 0; i < n; i++) {
        auto or_clauses = Or_1Bit("Pow_NBit_PowAccumOverflowAccum_" + Z(call_count) + "_" + Z(i),
                                 "Pow_NBit_Temp1Overflow_" + Z(call_count) + "_" + Z(i),
                                 "Pow_NBit_PowAccumOverflowAccum_" + Z(call_count) + "_" + Z(i+1)).expand();
        clauses.insert(clauses.end(), or_clauses.begin(), or_clauses.end());
    }
    
    // If_Cond_A_Else_B_1Bit for overflow temp (conditional overflow handling)
    for (int i = 0; i < n; i++) {
        auto if_clauses = If_Cond_A_Else_B_1Bit("Pow_NBit_PowAccumOverflowAccum_" + Z(call_count) + "_" + Z(i+1),
                                               "Zero_1Bit_" + Z(1),
                                               in_b + "_" + Z(i+1),
                                               "Pow_NBit_OverflowTemp_" + Z(call_count) + "_" + Z(i)).expand();
        clauses.insert(clauses.end(), if_clauses.begin(), if_clauses.end());
    }
    
    // Or_NBit_To_1Bit for pow accum overflow
    auto or_clauses = Or_NBit_To_1Bit("Pow_NBit_PowAccumOverflow_" + Z(call_count),
                                     "Pow_NBit_PowAccumOverflow_OR_" + Z(call_count),
                                     n).expand();
    clauses.insert(clauses.end(), or_clauses.begin(), or_clauses.end());
    
    // Or_NBit_To_1Bit for overflow temp
    auto or_temp_clauses = Or_NBit_To_1Bit("Pow_NBit_OverflowTemp_" + Z(call_count),
                                          "Pow_NBit_OverflowTemp_OR_" + Z(call_count),
                                          n).expand();
    clauses.insert(clauses.end(), or_temp_clauses.begin(), or_temp_clauses.end());
    
    // Or_1Bit for final overflow
    auto final_or_clauses = Or_1Bit("Pow_NBit_PowAccumOverflow_OR_" + Z(call_count),
                                  "Pow_NBit_OverflowTemp_OR_" + Z(call_count),
                                  over_flow).expand();
    clauses.insert(clauses.end(), final_or_clauses.begin(), final_or_clauses.end());
    
    return clauses;
}

/**
 * Class to represent double-size assignment: result[0...n] == in_a, result[n...(2*n)] == 0
 * Implements zero-extension of an N-bit value to 2N bits
 */
DoubleSize_Assign::DoubleSize_Assign(const std::string& in_a, const std::string& result, int n)
    : in_a(in_a), result(result), n(n) {}

std::vector<std::string> DoubleSize_Assign::expand() const {
    std::vector<std::string> clauses;
    
    // Equals_NBit for result[0...n] == in_a
    auto equals_clauses = Equals_NBit(in_a, result, n).expand();
    clauses.insert(clauses.end(), equals_clauses.begin(), equals_clauses.end());
    
    // Set result[n...(2*n)] to 0
    for (int i = n; i < (n * 2); i++) {
        clauses.push_back("-<" + result + "_" + Z(i) + "> 0 ");
    }
    
    return clauses;
}

/**
 * Class to represent modular exponentiation: result == (base ** exp) % mod
 * Implements fast modular exponentiation using repeated squaring
 */
PowMod_NBit::PowMod_NBit(const std::string& base, const std::string& exp, const std::string& mod, const std::string& result, int n)
    : base(base), exp(exp), mod(mod), result(result), n(n) {}

std::vector<std::string> PowMod_NBit::expand() const {
    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // DoubleSize_Assign for base, exp, and mod (extend to 2N bits for intermediate calculations)
    auto base_double_clauses = DoubleSize_Assign(base, "PowMod_NBit_Base_DoubleSize_" + Z(call_count), n).expand();
    clauses.insert(clauses.end(), base_double_clauses.begin(), base_double_clauses.end());
    
    auto exp_double_clauses = DoubleSize_Assign(exp, "PowMod_NBit_Exp_DoubleSize_" + Z(call_count), n).expand();
    clauses.insert(clauses.end(), exp_double_clauses.begin(), exp_double_clauses.end());
    
    auto mod_double_clauses = DoubleSize_Assign(mod, "PowMod_NBit_Mod_DoubleSize_" + Z(call_count), n).expand();
    clauses.insert(clauses.end(), mod_double_clauses.begin(), mod_double_clauses.end());
    
    // Initialize partial_result_0 = 1
    auto init_clauses = Input_Equals_Number("PowMod_NBit_PartialResult_" + Z(call_count) + "_" + Z(0), 1, n*2).expand();
    clauses.insert(clauses.end(), init_clauses.begin(), init_clauses.end());
    
    // Initialize current_pow_0 = base
    auto current_pow_clauses = Equals_NBit("PowMod_NBit_CurrentPow_" + Z(call_count) + "_" + Z(0),
                                         "PowMod_NBit_Base_DoubleSize_" + Z(call_count),
                                         n*2).expand();
    clauses.insert(clauses.end(), current_pow_clauses.begin(), current_pow_clauses.end());
    
    // For each bit in exp
    for (int i = 0; i < n; i++) {
        // bit_factor_i = if exp_i current_pow else 1
        auto bit_factor_clauses = If_Cond_A_Else_B_NBit("PowMod_NBit_CurrentPow_" + Z(call_count) + "_" + Z(i),
                                                       "One_NBit_" + Z(n*2),
                                                       "PowMod_NBit_Exp_DoubleSize_" + Z(call_count) + "_" + Z(i),
                                                       "PowMod_NBit_BitFactor_" + Z(call_count) + "_" + Z(i),
                                                       n*2).expand();
        clauses.insert(clauses.end(), bit_factor_clauses.begin(), bit_factor_clauses.end());
        
        // multipled_i = partial_result * bit_factor_i
        auto multipled_clauses = Mul_NBit("PowMod_NBit_PartialResult_" + Z(call_count) + "_" + Z(i),
                                        "PowMod_NBit_BitFactor_" + Z(call_count) + "_" + Z(i),
                                        "PowMod_NBit_Multipled_" + Z(call_count) + "_" + Z(i),
                                        "PowMod_NBit_MultipledOverflow_" + Z(call_count) + "_" + Z(i),
                                        n*2).expand();
        clauses.insert(clauses.end(), multipled_clauses.begin(), multipled_clauses.end());
        
        // partial_result_(i+1) = multipled_i % mod
        auto divmod_clauses = DivMod_NBit("PowMod_NBit_Multipled_" + Z(call_count) + "_" + Z(i),
                                        "PowMod_NBit_Mod_DoubleSize_" + Z(call_count),
                                        "PowMod_NBit_Div1_" + Z(call_count) + "_" + Z(i),
                                        "PowMod_NBit_PartialResult_" + Z(call_count) + "_" + Z(i+1),
                                        n*2).expand();
        clauses.insert(clauses.end(), divmod_clauses.begin(), divmod_clauses.end());
        
        // square_base_i = current_pow_i * current_pow_i
        auto square_clauses = Mul_NBit("PowMod_NBit_CurrentPow_" + Z(call_count) + "_" + Z(i),
                                     "PowMod_NBit_CurrentPow_" + Z(call_count) + "_" + Z(i),
                                     "PowMod_NBit_SquareBase_" + Z(call_count) + "_" + Z(i),
                                     "PowMod_NBit_SquareBaseOverflow_" + Z(call_count) + "_" + Z(i),
                                     n*2).expand();
        clauses.insert(clauses.end(), square_clauses.begin(), square_clauses.end());
        
        // current_pow_(i+1) = square_base_i % mod
        auto current_pow_next_clauses = DivMod_NBit("PowMod_NBit_SquareBase_" + Z(call_count) + "_" + Z(i),
                                                  "PowMod_NBit_Mod_DoubleSize_" + Z(call_count),
                                                  "PowMod_NBit_Div2_" + Z(call_count) + "_" + Z(i),
                                                  "PowMod_NBit_CurrentPow_" + Z(call_count) + "_" + Z(i+1),
                                                  n*2).expand();
        clauses.insert(clauses.end(), current_pow_next_clauses.begin(), current_pow_next_clauses.end());
    }
    
    // result = partial_result_n
    auto result_clauses = Equals_NBit(result,
                                    "PowMod_NBit_PartialResult_" + Z(call_count) + "_" + Z(n),
                                    n).expand();
    clauses.insert(clauses.end(), result_clauses.begin(), result_clauses.end());
    
    return clauses;
}

/**
 * Class to add a literal to each clause in a condition
 * Used for implementing logical operations on CNF conditions
 */
AddLiteralToCondition::AddLiteralToCondition(const std::string& literal, const std::vector<std::string>& condition)
    : literal(literal), condition(condition) {}

std::vector<std::string> AddLiteralToCondition::expand() const {
    std::vector<std::string> clauses;
    
    // Add the literal to each clause in the condition
    for (const auto& clause : condition) {
        clauses.push_back(literal + " " + clause);
    }
    
    return clauses;
}

/**
 * Class to represent logical OR between two conditions: condition1 || condition2
 * Implements disjunction using Tseitin transformation
 */
Or_Condition::Or_Condition(const std::vector<std::string>& condition1,
                          const std::vector<std::string>& condition2)
    : condition1(condition1), condition2(condition2) {}

std::vector<std::string> Or_Condition::expand() const {
    std::vector<std::string> clauses;
    static int call_count = 0;
    std::string or_literal = "<Or_Condition_" + Z(++call_count)+">";
    
    // Get expanded conditions from both function objects
    auto expanded_condition1 = condition1;
    auto expanded_condition2 = condition2;
    
    // Add literal to condition1 (positive)
    AddLiteralToCondition add_literal1(or_literal, expanded_condition1);
    auto new_clauses1 = add_literal1.expand();
    clauses.insert(clauses.end(), new_clauses1.begin(), new_clauses1.end());
    
    // Add negated literal to condition2
    std::string negated_literal = "-" + or_literal;
    AddLiteralToCondition add_literal2(negated_literal, expanded_condition2);
    auto new_clauses2 = add_literal2.expand();
    clauses.insert(clauses.end(), new_clauses2.begin(), new_clauses2.end());
    
    return clauses;
}

/**
 * Class to represent logical AND between two conditions: condition1 && condition2
 * Implements conjunction by combining all clauses from both conditions
 */
And_Condition::And_Condition(const std::vector<std::string>& condition1,
                           const std::vector<std::string>& condition2)
    : condition1(condition1), condition2(condition2) {}

std::vector<std::string> And_Condition::expand() const {
    std::vector<std::string> clauses;
    
    // Add all clauses from condition1
    clauses.insert(clauses.end(), condition1.begin(), condition1.end());
    
    // Add all clauses from condition2
    clauses.insert(clauses.end(), condition2.begin(), condition2.end());
    
    return clauses;
}

/**
 * Class to represent sum of multiple N-bit values: output == input_1 + input_2 + ... + input_(data_count)
 * Implements accumulation using repeated addition
 */
Sum_NBit::Sum_NBit(const std::string& input, const std::string& output,
                   const std::string& overflow, int data_count, int bits)
    : input(input), output(output), overflow(overflow), data_count(data_count), bits(bits) {}

std::vector<std::string> Sum_NBit::expand() const {
    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Initialize accumulator to 0
    Input_Equals_Number init_op("Sum_NBit_Accum_" + Z(call_count) + "_" + Z(0), 0, bits);
    auto init_clauses = init_op.expand();
    clauses.insert(clauses.end(), init_clauses.begin(), init_clauses.end());
    
    // Add each input to the accumulator
    for (int i = 0; i < data_count; i++) {
        Add_NBit add_op(
            input + "_" + Z(i),
            "Sum_NBit_Accum_" + Z(call_count) + "_" + Z(i),
            "Sum_NBit_Accum_" + Z(call_count) + "_" + Z(i + 1),
            "Sum_NBit_Overflow_" + Z(call_count) + "_" + Z(i),
            bits
        );
        auto add_clauses = add_op.expand();
        clauses.insert(clauses.end(), add_clauses.begin(), add_clauses.end());
    }
    
    // Set output equal to final accumulator value
    Equals_NBit equals_op(
        output,
        "Sum_NBit_Accum_" + Z(call_count) + "_" + Z(data_count),
        bits
    );
    auto equals_clauses = equals_op.expand();
    clauses.insert(clauses.end(), equals_clauses.begin(), equals_clauses.end());
    
    // Combine all overflow bits into a single overflow output
    Or_NBit_To_1Bit or_op(
        "Sum_NBit_Overflow_" + Z(call_count),
        overflow,
        data_count
    );
    auto or_clauses = or_op.expand();
    clauses.insert(clauses.end(), or_clauses.begin(), or_clauses.end());
    
    return clauses;
}

// Static member variable definition for Product_NBit
int Product_NBit::call_count = 0;

/**
 * Class to represent product of multiple N-bit values: output == input_1 * input_2 * ... * input_(data_count)
 * Implements accumulation using repeated multiplication
 */
Product_NBit::Product_NBit(const std::string& input, const std::string& output,
                          const std::string& overflow, int data_count, int bits)
    : input(input), output(output), overflow(overflow), data_count(data_count), bits(bits) {}

std::vector<std::string> Product_NBit::expand() const {
//    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Initialize accumulator to 1
    Input_Equals_Number init_op("Product_NBit_Accum_" + Z(call_count) + "_" + Z(0), 1, bits);
    auto init_clauses = init_op.expand();
    clauses.insert(clauses.end(), init_clauses.begin(), init_clauses.end());
    
    // Multiply each input with the accumulator
    for (int i = 0; i < data_count; i++) {
        Mul_NBit mul_op(
            input + "_" + Z(i),
            "Product_NBit_Accum_" + Z(call_count) + "_" + Z(i),
            "Product_NBit_Accum_" + Z(call_count) + "_" + Z(i + 1),
            "Product_NBit_Overflow_" + Z(call_count) + "_" + Z(i),
            bits
        );
        auto mul_clauses = mul_op.expand();
        clauses.insert(clauses.end(), mul_clauses.begin(), mul_clauses.end());
    }
    
    // Set output equal to final accumulator value
    Equals_NBit equals_op(
        output,
        "Product_NBit_Accum_" + Z(call_count) + "_" + Z(data_count),
        bits
    );
    auto equals_clauses = equals_op.expand();
    clauses.insert(clauses.end(), equals_clauses.begin(), equals_clauses.end());
    
    // Combine all overflow bits into a single overflow output
    Or_NBit_To_1Bit or_op(
        "Product_NBit_Overflow_" + Z(call_count),
        overflow,
        data_count
    );
    auto or_clauses = or_op.expand();
    clauses.insert(clauses.end(), or_clauses.begin(), or_clauses.end());
    
    return clauses;
}

// Static member variable definition for FermatTest
int FermatTest::call_count = 0;

/**
 * Class to represent Fermat primality test: (generator ** pow) % mod == 1
 * Implements the basic Fermat test for a given generator, power, and modulus
 */
FermatTest::FermatTest(const std::string& generator, const std::string& pow, 
                       const std::string& mod, int n)
    : generator(generator), pow(pow), mod(mod), n(n) {}

std::vector<std::string> FermatTest::expand() const {
//    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Input_Not_Equals_Number for generator != 0
    auto gen_not_zero_clause = Input_Not_Equals_Number(generator, 0, n).expand();
    clauses.push_back(gen_not_zero_clause);
    
    // Input_Not_Equals_Number for generator != 1
    auto gen_not_one_clause = Input_Not_Equals_Number(generator, 1, n).expand();
    clauses.push_back(gen_not_one_clause);
    
    // PowMod_NBit for (generator ** pow) % mod
    PowMod_NBit powmod_op(generator, pow, mod, "FermatTest_" + Z(call_count), n);
    auto powmod_clauses = powmod_op.expand();
    clauses.insert(clauses.end(), powmod_clauses.begin(), powmod_clauses.end());
    
    // Input_Equals_Number for result == 1
    auto result_equals_one_clauses = Input_Equals_Number("FermatTest_" + Z(call_count), 1, n).expand();
    clauses.insert(clauses.end(), result_equals_one_clauses.begin(), result_equals_one_clauses.end());
    
    return clauses;
}

// Static member variable definition for FermatTest2
int FermatTest2::call_count = 0;

/**
 * Class to represent Fermat primality test for prime: (generator ** (prime-1)) % prime == 1
 * Implements the standard Fermat test used in primality testing
 */
FermatTest2::FermatTest2(const std::string& generator, const std::string& prime, int n)
    : generator(generator), prime(prime), n(n) {}

std::vector<std::string> FermatTest2::expand() const {
//    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Add_NBit for prime - 1
    Add_NBit add_op("FermatTest2_Prime_Minus1_" + Z(call_count),
                    "One_NBit_" + Z(n),
                    prime,
                    "FermatTest2_Prime_Minus1_Overflow_" + Z(call_count),
                    n);
    auto add_clauses = add_op.expand();
    clauses.insert(clauses.end(), add_clauses.begin(), add_clauses.end());
    
    // Ensure no overflow in the subtraction
    clauses.push_back("-<FermatTest2_Prime_Minus1_Overflow_" + Z(call_count) + "> 0 ");
    
    // FermatTest for (generator ** (prime-1)) % prime == 1
    FermatTest fermat_op(generator,
                        "FermatTest2_Prime_Minus1_" + Z(call_count),
                        prime,
                        n);
    auto fermat_clauses = fermat_op.expand();
    clauses.insert(clauses.end(), fermat_clauses.begin(), fermat_clauses.end());
    
    return clauses;
}

// Static member variable definition for FermatTest3
int FermatTest3::call_count = 0;

/**
 * Class to represent inverse Fermat test: (generator ** pow) % mod != 1
 * Implements the negation of Fermat test, used for composite number detection
 */
FermatTest3::FermatTest3(const std::string& generator, const std::string& pow, 
                         const std::string& mod, int n)
    : generator(generator), pow(pow), mod(mod), n(n) {}

std::vector<std::string> FermatTest3::expand() const {
//    static int call_count = 0;
    call_count++;
    
    std::vector<std::string> clauses;
    
    // Input_Not_Equals_Number for generator != 0
    auto gen_not_zero_clause = Input_Not_Equals_Number(generator, 0, n).expand();
    clauses.push_back(gen_not_zero_clause);
    
    // Input_Not_Equals_Number for generator != 1
    auto gen_not_one_clause = Input_Not_Equals_Number(generator, 1, n).expand();
    clauses.push_back(gen_not_one_clause);
    
    // PowMod_NBit for (generator ** pow) % mod
    PowMod_NBit powmod_op(generator, pow, mod, "FermatTest3_" + Z(call_count), n);
    auto powmod_clauses = powmod_op.expand();
    clauses.insert(clauses.end(), powmod_clauses.begin(), powmod_clauses.end());
    
    // Input_Not_Equals_Number for result != 1
    auto result_not_one_clause = Input_Not_Equals_Number("FermatTest3_" + Z(call_count), 1, n).expand();
    clauses.push_back(result_not_one_clause);
    
    return clauses;
} 