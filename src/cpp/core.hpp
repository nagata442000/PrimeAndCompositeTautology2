// core.hpp - Core logic for CNF generation and arithmetic/logic operations for SAT-based number theory problems.
//
// This file defines classes for encoding arithmetic, logic, and number-theoretic constraints as CNF clauses.
// Each class provides an expand() method to generate CNF clauses for a specific operation or property.
//
#pragma once
#include <string>
#include <vector>

// Returns a zero-padded string representation of an integer (used for variable naming)
std::string Z(int i);

// Constraint: input == value (bitwise equality)
class Input_Equals_Number {
private:
    std::string input;
    int value;
    int n;
public:
    Input_Equals_Number(const std::string& input, int value, int n);
    std::vector<std::string> expand() const;
};

// Constraint: input != value (bitwise inequality)
class Input_Not_Equals_Number {
private:
    std::string input;
    int value;
    int n;
public:
    Input_Not_Equals_Number(const std::string& input, int value, int n);
    std::string expand() const;
};

// Constraint: carry_out == (popcount(in_a, in_b, carry_in) >= 2)
class CarryOut_Equal_POPCNT_GREATER_THAN_2 {
private:
    std::string in_a;
    std::string in_b;
    std::string carry_in;
    std::string carry_out;
public:
    CarryOut_Equal_POPCNT_GREATER_THAN_2(const std::string& in_a, const std::string& in_b, 
                                        const std::string& carry_in, const std::string& carry_out);
    std::vector<std::string> expand() const;
};

// Constraint: result == in_a ^ in_b ^ carry_in (1-bit addition result)
class Result_Equal_A_XOR_B_XOR_CarryIn {
private:
    std::string in_a;
    std::string in_b;
    std::string carry_in;
    std::string result;
public:
    Result_Equal_A_XOR_B_XOR_CarryIn(const std::string& in_a, const std::string& in_b, 
                                     const std::string& carry_in, const std::string& result);
    std::vector<std::string> expand() const;
};

// Constraint: 1-bit full adder (in_a + in_b + carry_in == (result, carry_out))
class Add_1Bit {
private:
    std::string in_a;
    std::string in_b;
    std::string carry_in;
    std::string result;
    std::string carry_out;
public:
    Add_1Bit(const std::string& in_a, const std::string& in_b, 
             const std::string& carry_in, const std::string& result, const std::string& carry_out);
    std::vector<std::string> expand() const;
};

// Constraint: n-bit adder (in_a + in_b == result, with overflow)
class Add_NBit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    std::string over_flow;
    int n;
    static int call_count;
public:
    Add_NBit(const std::string& in_a, const std::string& in_b, 
             const std::string& result, const std::string& over_flow, int n);
    std::vector<std::string> expand() const;
};

// Constraint: (in_a * in_b) << shift == result (partial product for multiplication)
class Mul_NBit_1Bit_Shift {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    int shift;
    int n;
    static int call_count;
public:
    Mul_NBit_1Bit_Shift(const std::string& in_a, const std::string& in_b, 
                        const std::string& result, int shift, int n);
    std::vector<std::string> expand() const;
};

// Constraint: n-bit multiplier (in_a * in_b == result, with overflow)
class Mul_NBit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    std::string over_flow;
    int n;
    static int call_count;
public:
    Mul_NBit(const std::string& in_a, const std::string& in_b, 
             const std::string& result, const std::string& over_flow, int n);
    std::vector<std::string> expand() const;
};

// Constraint: n-bit multiplication by a single bit (in_a * in_b == result)
class Mul_NBit_1Bit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    int n;
    static int call_count;
public:
    Mul_NBit_1Bit(const std::string& in_a, const std::string& in_b, 
                  const std::string& result, int n);
    std::vector<std::string> expand() const;
};

// Constraint: Encodes primality of a number using number-theoretic CNF
class IsPrime {
private:
    std::string target;
    int n;
    int num_prime;
    static int call_count;

public:
    IsPrime(const std::string& target, int n, int num_prime );
    std::vector<std::string> expand() const;
};

// Constraint: Encodes compositeness of a number using number-theoretic CNF
class IsComposite {
private:
    std::string target;
    int n;
public:
    IsComposite(const std::string& target, int n);
    std::vector<std::string> expand() const;
};

// Constraint: result == in_a & in_b (bitwise AND)
class And_1Bit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    static int call_count;
public:
    And_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result);
    std::vector<std::string> expand() const;
};

// Constraint: result == (in_a < in_b) (1-bit less-than)
class LessThan_1Bit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    static int call_count;
public:
    LessThan_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result);
    std::vector<std::string> expand() const;
};

// Constraint: result == (in_a == in_b) (1-bit equality)
class Equals_1Bit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    static int call_count;
public:
    Equals_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result);
    std::vector<std::string> expand() const;
};

// Constraint: n-bit equality (in_a == in_b)
class Equals_NBit {
private:
    std::string in_a;
    std::string in_b;
    int n;
    static int call_count;
public:
    Equals_NBit(const std::string& in_a, const std::string& in_b, int n);
    std::vector<std::string> expand() const;
};

// Constraint: n-bit less-than (in_a < in_b)
class LessThan_NBit {
private:
    std::string in_a;
    std::string in_b;
    int n;
    static int call_count;
public:
    LessThan_NBit(const std::string& in_a, const std::string& in_b, int n);
    std::vector<std::string> expand() const;
};

// Constraint: n-bit division and modulo (in_a == in_b * div + mod)
class DivMod_NBit {
private:
    std::string in_a;
    std::string in_b;
    std::string div;
    std::string mod;
    int n;
    static int call_count;
public:
    DivMod_NBit(const std::string& in_a, const std::string& in_b, 
                const std::string& div, const std::string& mod, int n);
    std::vector<std::string> expand() const;
};

// Constraint: result == if cond then a else b (1-bit conditional)
class If_Cond_A_Else_B_1Bit {
private:
    std::string in_a;
    std::string in_b;
    std::string cond;
    std::string result;
    static int call_count;
public:
    If_Cond_A_Else_B_1Bit(const std::string& in_a, const std::string& in_b, 
                          const std::string& cond, const std::string& result);
    std::vector<std::string> expand() const;
};

// Constraint: result == if cond then a else b (n-bit conditional)
class If_Cond_A_Else_B_NBit {
private:
    std::string in_a;
    std::string in_b;
    std::string cond;
    std::string result;
    int n;
    static int call_count;
public:
    If_Cond_A_Else_B_NBit(const std::string& in_a, const std::string& in_b, 
                          const std::string& cond, const std::string& result, int n);
    std::vector<std::string> expand() const;
};

// Constraint: result == in_a | in_b (bitwise OR)
class Or_1Bit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
public:
    Or_1Bit(const std::string& in_a, const std::string& in_b, const std::string& result);
    std::vector<std::string> expand() const;
};

// Constraint: result == OR of n input bits
class Or_NBit_To_1Bit {
private:
    std::string in_a;
    std::string result;
    int n;
public:
    Or_NBit_To_1Bit(const std::string& in_a, const std::string& result, int n);
    std::vector<std::string> expand() const;
};

// Constraint: result == in_a ** in_b (n-bit exponentiation)
class Pow_NBit {
private:
    std::string in_a;
    std::string in_b;
    std::string result;
    std::string over_flow;
    int n;
public:
    Pow_NBit(const std::string& in_a, const std::string& in_b, const std::string& result, const std::string& over_flow, int n);
    std::vector<std::string> expand() const;
};

// Constraint: result[0...n] == in_a, result[n...(2*n)] == 0 (zero-extended assignment)
class DoubleSize_Assign {
private:
    std::string in_a;
    std::string result;
    int n;
public:
    DoubleSize_Assign(const std::string& in_a, const std::string& result, int n);
    std::vector<std::string> expand() const;
};

// Constraint: result == (base ** exp) % mod (modular exponentiation)
class PowMod_NBit {
private:
    std::string base;
    std::string exp;
    std::string mod;
    std::string result;
    int n;
public:
    PowMod_NBit(const std::string& base, const std::string& exp, const std::string& mod, const std::string& result, int n);
    std::vector<std::string> expand() const;
};

// Utility: Adds a literal to all clauses in a condition
class AddLiteralToCondition {
private:
    std::string literal;
    std::vector<std::string> condition;
public:
    AddLiteralToCondition(const std::string& literal, const std::vector<std::string>& condition);
    std::vector<std::string> expand() const;
};

// Utility: Logical OR of two CNF conditions
class Or_Condition {
private:
    std::vector<std::string> condition1;
    std::vector<std::string> condition2;

public:
    Or_Condition(const std::vector<std::string>& condition1,
                 const std::vector<std::string>& condition2);
    std::vector<std::string> expand() const;
};

// Utility: Logical AND of two CNF conditions
class And_Condition {
private:
    std::vector<std::string> condition1;
    std::vector<std::string> condition2;

public:
    And_Condition(const std::vector<std::string>& condition1,
                  const std::vector<std::string>& condition2);
    std::vector<std::string> expand() const;
};

// Constraint: output == sum of data_count n-bit inputs
class Sum_NBit {
private:
    std::string input;
    std::string output;
    std::string overflow;
    int data_count;
    int bits;

public:
    Sum_NBit(const std::string& input, const std::string& output,
             const std::string& overflow, int data_count, int bits);
    std::vector<std::string> expand() const;
};

// Constraint: output == product of data_count n-bit inputs
class Product_NBit {
private:
    std::string input;
    std::string output;
    std::string overflow;
    int data_count;
    int bits;
    static int call_count;

public:
    Product_NBit(const std::string& input, const std::string& output,
                 const std::string& overflow, int data_count, int bits);
    std::vector<std::string> expand() const;
};

// Constraint: Fermat's little theorem test (generator^pow % mod == 1)
class FermatTest {
private:
    std::string generator;
    std::string pow;
    std::string mod;
    int n;
    static int call_count;

public:
    FermatTest(const std::string& generator, const std::string& pow, 
               const std::string& mod, int n);
    std::vector<std::string> expand() const;
};

// Constraint: Fermat's test for prime-1 (generator^(prime-1) % prime == 1)
class FermatTest2 {
private:
    std::string generator;
    std::string prime;
    int n;
    static int call_count;

public:
    FermatTest2(const std::string& generator, const std::string& prime, int n);
    std::vector<std::string> expand() const;
};

// Constraint: Fermat's test for non-1 (generator^pow % mod != 1)
class FermatTest3 {
private:
    std::string generator;
    std::string pow;
    std::string mod;
    int n;
    static int call_count;

public:
    FermatTest3(const std::string& generator, const std::string& pow, 
                const std::string& mod, int n);
    std::vector<std::string> expand() const;
};

// Generates a CNF file from a set of conditions
void generate_cnf(const std::vector<std::string>& conditions, const std::string& file_path); 